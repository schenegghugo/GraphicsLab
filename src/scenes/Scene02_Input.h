#pragma once
#include "Scene.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Scene02_Input : public Scene {
    float x, y;
    float speed;

public:
    Scene02_Input() : x(0.0f), y(0.0f), speed(50.0f) {}

    std::string GetName() const override { return "Scene 02: Keyboard Input"; }

    void OnAttach() override {
        // Set background to Dark Grey
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        std::cout << "Controls: W/A/S/D to move the square." << std::endl;
    }

    void OnUpdate(float dt) override {
        GLFWwindow* window = glfwGetCurrentContext();

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) y += speed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) y -= speed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) x += speed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) x -= speed;
    }

    void OnRender() override {
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a Red Box
        glEnable(GL_SCISSOR_TEST);
        glScissor((int)x + 640, (int)y + 360, 50, 50);
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // RED
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);      
        // Reset background color
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    }
};


