#pragma once
#include "Scene.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

class Scene03_MouseInput : public Scene {
public:
    void OnAttach() override {
        std::cout << "Scene 03 Loaded: Move your mouse!" << std::endl;
    }

    void OnUpdate(float deltaTime) override {
        // No manual update needed, we poll in Render
    }

    void OnRender() override {
        GLFWwindow* window = glfwGetCurrentContext();

        // 1. Get Mouse Position
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // 2. Get Window Size
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // --- COORDINATE FIX ---
        // GLFW (Window System): (0,0) is TOP-Left.
        // OpenGL (Rendering):   (0,0) is BOTTOM-Left.
        // We calculate glMouseY to match OpenGL coordinates.
        int glMouseY = height - (int)mouseY;

        // Calculate a 0.0 to 1.0 ratio based on height
        // 1.0 = Top (Day), 0.0 = Bottom (Night)
        float heightRatio = (float)glMouseY / height;
        // Clamp ratio to 0-1 just in case mouse goes off screen
        if (heightRatio < 0.0f) heightRatio = 0.0f;
        if (heightRatio > 1.0f) heightRatio = 1.0f;

        // 3. Clear Background
        // Blue when high (Top), Dark when low (Bottom)
        glClearColor(0.1f, 0.1f, heightRatio, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_SCISSOR_TEST);

        // --- FEATURE: STARS ---
        // If we are in the bottom half of the screen (Dark/Night)
        if (heightRatio < 0.5f) 
        {
            // Set star color to White
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

            // Draw 20 fixed stars
            for (int i = 0; i < 20; i++) {
                // Pseudo-random math using 'i' so stars don't flicker
                int starX = (i * 137) % width;           
                // Keep stars in the top 200 pixels of the screen (OpenGL Y is bottom-up, so height - offset)
                int starY = height - 10 - ((i * 53) % 200); 
                
                // Draw a tiny 2x2 box for each star
                glScissor(starX, starY, 2, 2); 
                glClear(GL_COLOR_BUFFER_BIT);
            }
        }

        // 4. Calculate Box Size based on Y Axis
        // Logic: When cursor is Top (Ratio 1.0) -> Size 150
        //        When cursor is Bottom (Ratio 0.0) -> Size 20
        float maxSize = 150.0f;
        float minSize = 0.0f;
        
        // Linear Interpolation: Size grows as ratio grows
        int boxSize = (int)(minSize + (maxSize - minSize) * heightRatio);

        // 5. Draw the Gold Box centered on Mouse
        glScissor((int)mouseX - (boxSize/2), glMouseY - (boxSize/2), boxSize, boxSize);
        
        glClearColor(1.0f, 0.8f, 0.2f, 1.0f); // Gold color
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_SCISSOR_TEST);
    }

    std::string GetName() const override { return "Scene 03: Mouse & Pulse"; }
};
