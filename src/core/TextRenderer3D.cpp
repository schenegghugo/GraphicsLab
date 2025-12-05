#include "TextRenderer3D.h"

// --- STANDARD LIBRARY INCLUDES ---
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <array>         // <--- REQUIRED: Fixes "incomplete type std::array"
#include <algorithm>

// --- GLM EXTENSIONS ---
#include <glm/gtc/matrix_transform.hpp> // <--- REQUIRED: Fixes glm::translate/scale
#include <glm/gtc/type_ptr.hpp>         // <--- REQUIRED: Fixes glm::value_ptr

// --- IMPLEMENTATION HEADERS ---
#define STB_TRUETYPE_IMPLEMENTATION
#include "../libs/stb_truetype.h"

#include "../libs/earcut.hpp"

// Define the point type for Earcut (Must happen before usage)
namespace mapbox {
namespace util {
template <> struct nth<0, std::array<double, 2>> {
    inline static double get(const std::array<double, 2> &t) { return t[0]; };
};
template <> struct nth<1, std::array<double, 2>> {
    inline static double get(const std::array<double, 2> &t) { return t[1]; };
};
}
}

TextRenderer3D::TextRenderer3D() {}

TextRenderer3D::~TextRenderer3D() {
    // If the header <map> is missing, this line fails. 
    // Since we fixed the header, this will now work.
    for (auto& pair : m_Glyphs) {
        glDeleteVertexArrays(1, &pair.second.VAO);
        glDeleteBuffers(1, &pair.second.VBO);
    }
}

// --------------------------------------------------------
// HELPER: Add Point (Fixing the brace initialization error)
// --------------------------------------------------------
void AddPoint(std::vector<std::array<double, 2>>& poly, float x, float y) {
    if (poly.empty()) {
        poly.push_back(std::array<double, 2>{(double)x, (double)y});
        return;
    }
    // Check duplicates
    std::array<double, 2>& last = poly.back();
    if (std::abs(last[0] - x) > 0.001 || std::abs(last[1] - y) > 0.001) {
        poly.push_back(std::array<double, 2>{(double)x, (double)y});
    }
}

void FlattenCurve(std::vector<std::array<double, 2>>& poly, float x1, float y1, float cx, float cy, float x2, float y2, int depth = 0) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float d = std::abs((cx - x2) * dy - (cy - y2) * dx);

    if (d < 0.5f || depth > 5) { 
        AddPoint(poly, x2, y2);
        return;
    }
    
    float x12 = (x1 + cx) / 2;
    float y12 = (y1 + cy) / 2;
    float x23 = (cx + x2) / 2;
    float y23 = (cy + y2) / 2;
    float x123 = (x12 + x23) / 2;
    float y123 = (y12 + y23) / 2;

    FlattenCurve(poly, x1, y1, x12, y12, x123, y123, depth + 1);
    FlattenCurve(poly, x123, y123, x23, y23, x2, y2, depth + 1);
}

// --------------------------------------------------------
// MESH GENERATION
// --------------------------------------------------------
void TextRenderer3D::CreateGlyphMesh(char c, const void* infoPtr) {
    const stbtt_fontinfo* info = (const stbtt_fontinfo*)infoPtr;
    int glyphIndex = stbtt_FindGlyphIndex(info, c);
    
    if (glyphIndex == 0) return;

    stbtt_vertex* verts;
    int numVerts = stbtt_GetGlyphShape(info, glyphIndex, &verts);
    
    using Point = std::array<double, 2>;
    using Polygon = std::vector<std::vector<Point>>;
    Polygon polygon;
    
    float startX = 0, startY = 0;
    float curX = 0, curY = 0;

    for (int i = 0; i < numVerts; ++i) {
        if (verts[i].type == STBTT_vmove) {
            polygon.push_back(std::vector<Point>());
            startX = verts[i].x; startY = verts[i].y;
            curX = startX; curY = startY;
            AddPoint(polygon.back(), curX, curY);
        }
        else if (verts[i].type == STBTT_vline) {
            curX = verts[i].x; curY = verts[i].y;
            AddPoint(polygon.back(), curX, curY);
        }
        else if (verts[i].type == STBTT_vcurve) {
            FlattenCurve(polygon.back(), curX, curY, verts[i].cx, verts[i].cy, verts[i].x, verts[i].y);
            curX = verts[i].x; curY = verts[i].y;
        }
    }
    stbtt_FreeShape(info, verts);

    if (polygon.empty()) return;

    // 1. Triangulate Front Face
    std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(polygon);

    // 2. Build 3D Mesh Data
    std::vector<float> meshData;

    auto addVert = [&](float x, float y, float z, float nx, float ny, float nz) {
        meshData.push_back(x); meshData.push_back(y); meshData.push_back(z);
        meshData.push_back(nx); meshData.push_back(ny); meshData.push_back(nz);
    };

    // -- FRONT FACE (Z = 0) --
    for (const auto& ring : polygon) {
        for (const auto& p : ring) {
            addVert((float)p[0], (float)p[1], 0.0f, 0, 0, 1);
        }
    }

    // -- BACK FACE (Z = -1) --
    int baseBack = meshData.size() / 6; 
    for (const auto& ring : polygon) {
        for (const auto& p : ring) {
            addVert((float)p[0], (float)p[1], -1.0f, 0, 0, -1);
        }
    }

    // -- SIDES --
    std::vector<uint32_t> sideIndices;
    int ringOffset = 0;
    
    for (const auto& ring : polygon) {
        int ringSize = ring.size();
        for (int i = 0; i < ringSize; ++i) {
            int current = ringOffset + i;
            int next = ringOffset + ((i + 1) % ringSize);
            
            int currentBack = baseBack + current;
            int nextBack = baseBack + next;

            // Compute Normal
            float dx = (float)(ring[(i+1)%ringSize][0] - ring[i][0]);
            float dy = (float)(ring[(i+1)%ringSize][1] - ring[i][1]);
            float len = sqrt(dx*dx + dy*dy);
            float nx = dy/len; 
            float ny = -dx/len;

            // Note: For perfect flat shading we should duplicate verts here with new normals.
            // For now, we connect existing verts. This creates "smooth" looking corners.
            // Since we use flat color shader, it's acceptable.

            sideIndices.push_back(current);
            sideIndices.push_back(next);
            sideIndices.push_back(currentBack);
            
            sideIndices.push_back(next);
            sideIndices.push_back(nextBack);
            sideIndices.push_back(currentBack);
        }
        ringOffset += ringSize;
    }

    std::vector<uint32_t> finalIndices = indices;

    // Reverse Back Face indices
    for(size_t i = 0; i < indices.size(); i+=3) {
        finalIndices.push_back(baseBack + indices[i]);
        finalIndices.push_back(baseBack + indices[i+2]);
        finalIndices.push_back(baseBack + indices[i+1]);
    }

    finalIndices.insert(finalIndices.end(), sideIndices.begin(), sideIndices.end());

    // Upload
    GlyphMesh gm;
    gm.indexCount = finalIndices.size();
    
    int advWidth, lsb;
    stbtt_GetCodepointHMetrics(info, c, &advWidth, &lsb);
    gm.advance = (float)advWidth;

    glGenVertexArrays(1, &gm.VAO);
    glGenBuffers(1, &gm.VBO);
    GLuint EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(gm.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, gm.VBO);
    glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(float), meshData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, finalIndices.size() * sizeof(uint32_t), finalIndices.data(), GL_STATIC_DRAW);

    // Pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    m_Glyphs[c] = gm;
}

bool TextRenderer3D::LoadFont(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    m_FontBuffer.resize(size);
    if (!file.read((char*)m_FontBuffer.data(), size)) return false;

    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, m_FontBuffer.data(), 0)) return false;

    std::cout << "Generating 3D meshes for font..." << std::endl;
    for (char c = 32; c < 127; c++) {
        CreateGlyphMesh(c, &info);
    }
    std::cout << "Done generating." << std::endl;

    return true;
}

void TextRenderer3D::RenderText(const std::string& text, float x, float y, float scale, float depth, 
                                GLuint shader, const float* mat4Value) {
    glUseProgram(shader);
    
    GLint loc = glGetUniformLocation(shader, "uMVP");
    
    float cursorX = x;

    // Convert raw pointer to GLM for manipulation
    glm::mat4 baseMatrix = glm::make_mat4(mat4Value);

    for (char c : text) {
        if (m_Glyphs.find(c) == m_Glyphs.end()) continue;
        
        GlyphMesh& gm = m_Glyphs[c];
        
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(cursorX, y, 0.0f));
        glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, depth)); 
        
        glm::mat4 finalMat = baseMatrix * model * scaling;
        
        glUniformMatrix4fv(loc, 1, GL_FALSE, &finalMat[0][0]);
        
        glBindVertexArray(gm.VAO);
        glDrawElements(GL_TRIANGLES, gm.indexCount, GL_UNSIGNED_INT, 0);
        
        cursorX += gm.advance * scale;
    }
    glBindVertexArray(0);
}
