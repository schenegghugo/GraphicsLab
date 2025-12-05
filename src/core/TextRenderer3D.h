#pragma once

#include <string>
#include <vector>
#include <map>           // <--- Critical: Defines std::map
#include <GL/glew.h>
#include <glm/glm.hpp>

// 1. Define the structure for the letter mesh
struct GlyphMesh {
    GLuint VAO, VBO;
    int indexCount;
    float advance; 
    float minX, minY, maxX, maxY; 
};

// 2. Define the class
class TextRenderer3D {
private:
    // This map stores the loaded 3D letters. 
    // If this line is missing in your file, the .cpp file will fail.
    std::map<char, GlyphMesh> m_Glyphs;
    
    std::vector<unsigned char> m_FontBuffer; 
    float m_ExtrusionDepth = 10.0f; 

    // Helper function definition
    void CreateGlyphMesh(char c, const void* fontInfo);

public:
    TextRenderer3D();
    ~TextRenderer3D();

    bool LoadFont(const std::string& path);
    
    void RenderText(const std::string& text, float x, float y, float scale, float depth, 
                    GLuint shaderProgram, const float* transformMatrix);
};
