#pragma once
#include "Scene.h"
#include "../core/TextRenderer3D.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath> // Required for sin()

// --- VERTEX SHADER (Adapted for TextRenderer3D) ---
static const char* klaffaVert = R"GLSL(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
// Note: TextRenderer3D doesn't send UVs on loc 2, so we calculate them

out vec3 vNormal;
out vec2 vUV;
out float vDepth;

uniform mat4 uMVP;      // Combined Matrix from Renderer
uniform mat4 uModel;    // Global Model Rotation
uniform float extrusionDepth;

void main()
{
    // 1. Position
    gl_Position = uMVP * vec4(aPos, 1.0);

    // 2. Normal (Rotate by the global model matrix)
    vNormal = mat3(uModel) * aNormal;

    // 3. Generate UVs based on position for the gradient effect
    // Scaling factor ensures the pattern isn't too tiny
    vUV = aPos.xy * 0.5; 

    // 4. Depth map for the dark purple shadow blending
    // Map Z from [-1..0] to [0..1] roughly
    vDepth = abs(aPos.z);
}
)GLSL";

// --- FRAGMENT SHADER (Your Klaffa Shader) ---
static const char* klaffaFrag = R"GLSL(
#version 330 core

in vec3 vNormal;
in vec2 vUV;
in float vDepth;

out vec4 FragColor;

// palette
const vec3 COL_SHADOW = vec3(0.188, 0.003, 0.314);   // #300350
const vec3 COL_DARKPINK = vec3(0.580, 0.086, 0.498); // #94167F
const vec3 COL_PINK = vec3(0.914, 0.203, 0.475);     // #E93479
const vec3 COL_ORANGE = vec3(0.976, 0.675, 0.325);   // #F9AC53

// halftone parameters
const float DOT_SCALE = 0.8;  // Adjusted scale for 3D text
const float DOT_THRESHOLD = 0.55;

float halftone(vec2 uv)
{
    // Rotate pattern slightly
    float s = sin(0.5); float c = cos(0.5);
    mat2 rot = mat2(c, -s, s, c);
    uv = rot * uv;

    uv *= 80.0; // Density

    vec2 cell = fract(uv);
    vec2 center = cell - 0.5;

    float dist = length(center) * 2.0;

    return smoothstep(DOT_THRESHOLD, DOT_THRESHOLD - 0.1, dist);
}

void main()
{
    // Light calculation for slight 3D feel
    vec3 lightDir = normalize(vec3(0.5, 0.5, 1.0));
    float diff = max(dot(normalize(vNormal), lightDir), 0.0);

    // base gradient (top -> bottom)
    float t = clamp(vUV.y + 0.5, 0.0, 1.0);

    vec3 topBlend = mix(COL_PINK, COL_ORANGE, t);
    vec3 baseColor = mix(COL_DARKPINK, topBlend, t);

    // halftone
    float dotMask = halftone(vUV);

    // darken where dots occur
    vec3 halftoned = mix(baseColor * 0.75, baseColor, dotMask);

    // shadow / extrusion color when depth is low (Z is negative)
    // We blend based on vDepth calculated in vertex shader
    float depthMix = smoothstep(0.0, 0.5, vDepth);
    vec3 finalColor = mix(halftoned, COL_SHADOW, depthMix);

    FragColor = vec4(finalColor, 1.0);
}
)GLSL";

// --- C++ CLASS DEFINITION ---
class Scene04_Optimized : public Scene {
    TextRenderer3D m_TextSystem;
    GLuint m_Shader;

public:
    void OnAttach() override {
        // 1. Load Font
        std::string fontPath = "/home/hugo/Work/resources/font/ttf/LineLineShapeDirty.ttf";
        if(m_TextSystem.LoadFont(fontPath)) {
            std::cout << "Scene04: Loaded font: " << fontPath << std::endl;
        } else {
            std::cerr << "Scene04: ERROR - Could not find font at: " << fontPath << std::endl;
        }

        // 2. Compile Shaders
        GLuint vs = glCreateShader(GL_VERTEX_SHADER); 
        glShaderSource(vs, 1, &klaffaVert, NULL); 
        glCompileShader(vs);
        
        // Check VS errors
        GLint success; char infoLog[512];
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if(!success) { glGetShaderInfoLog(vs, 512, NULL, infoLog); std::cerr << "VS Error: " << infoLog << std::endl; }

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER); 
        glShaderSource(fs, 1, &klaffaFrag, NULL); 
        glCompileShader(fs);

        // Check FS errors
        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if(!success) { glGetShaderInfoLog(fs, 512, NULL, infoLog); std::cerr << "FS Error: " << infoLog << std::endl; }

        m_Shader = glCreateProgram(); 
        glAttachShader(m_Shader, vs); 
        glAttachShader(m_Shader, fs); 
        glLinkProgram(m_Shader);
    }

    void OnUpdate(float dt) override {}

    void OnRender() override {
        glClearColor(0.188f, 0.003f, 0.314f, 1.0f); // Match Shadow Color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glUseProgram(m_Shader);
        
        int w, h; glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);
        float aspect = (float)w / (float)h;
        
        // --- MATRICES ---
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 15), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        
        float time = (float)glfwGetTime();
        
        // Global Rotation
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f)); 
        
        // --- FIX: Cast sin() result to float to match GLM types ---
        model = glm::rotate(model, glm::radians(10.0f) * (float)sin(time), glm::vec3(1.0f, 0.0f, 0.0f));

        // Pass 'model' explicitly for lighting calculation
        glUniformMatrix4fv(glGetUniformLocation(m_Shader, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform1f(glGetUniformLocation(m_Shader, "extrusionDepth"), 1.0f);

        // Calculate Base MVP (Projection * View * GlobalModel)
        // TextRenderer will add Local Translation/Scale to this
        glm::mat4 mvpBase = projection * view * model;

        // Render Text
        // Scale 0.005, Depth 1.0
        m_TextSystem.RenderText("KLAPPA", -4.0f, -0.5f, 0.005f, 1.0f, m_Shader, glm::value_ptr(mvpBase));
    }

    std::string GetName() const override { return "Scene 04: Klaffa Style"; }
};
