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
        // We don't need manual logic updates here 
        // because we will poll the mouse directly during render
        // for maximum responsiveness.
    }

    void OnRender() override {
        GLFWwindow* window = glfwGetCurrentContext();

        // 1. Get Mouse Position
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // 2. Get Window Size (in case you resized it)
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // --- THE GRAPHICS TRAP ---
        // GLFW (Window System): (0,0) is TOP-Left.
        // OpenGL (Rendering):   (0,0) is BOTTOM-Left.
        // We must flip the Y axis!
        int glMouseY = height - (int)mouseY;

        // 3. Clear Background (Dark Blue-ish)
        // We change background based on X position!
        float bgBlue = (float)mouseX / width; 
        glClearColor(0.1f, 0.1f, bgBlue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 4. Calculate Pulsing Size
        float time = (float)glfwGetTime();
        // sin goes -1 to 1. We want size 50 to 150.
        int boxSize = (int)((sin(time * 5.0f) * 50.0f) + 100.0f);

        // 5. Draw the Box centered on Mouse
        glEnable(GL_SCISSOR_TEST);
        
        // (X, Y, Width, Height) -> We offset X and Y by half size to center it
        glScissor((int)mouseX - (boxSize/2), glMouseY - (boxSize/2), boxSize, boxSize);
        
        // Box Color (Inverted from background)
        glClearColor(1.0f, 0.8f, 0.2f, 1.0f); // Gold color
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_SCISSOR_TEST);
    }

    std::string GetName() const override { return "Scene 03: Mouse & Pulse"; }
};
