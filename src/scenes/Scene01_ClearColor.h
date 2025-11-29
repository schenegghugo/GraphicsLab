#pragma once
#include "Scene.h"
#include <GL/glew.h>
#include <cmath>

class Scene01_ClearColor : public Scene {
public:
    void OnAttach() override {
        // Run once setup
    }

    void OnUpdate(float deltaTime) override {
        // Logic updates if needed
    }

    void OnRender() override {
        // Cycle background color over time
        float time = (float)glfwGetTime();
        float r = (sin(time) / 2.0f) + 0.5f;
        float g = (cos(time) / 2.0f) + 0.5f;
        float b = (sin(time) / 2.0f) + 0.5f;
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    std::string GetName() const override { return "Scene 01: Clear Color"; }
};
