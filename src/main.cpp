#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

// --- SCENE HEADERS ---
#include "scenes/Scene.h" 
#include "scenes/Scene01_ClearColor.h"
#include "scenes/Scene02_Input.h"       // Added this (was missing!)
#include "scenes/Scene03_MouseInput.h"  // Contains class Scene03

// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

int main() {
    // 1. Init GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 2. Configure for OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 3. Create Window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GraphicsLab [Arch]", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 4. Init GLEW
    glewExperimental = GL_TRUE; 
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // --------------------------------------
    // INITIALIZATION
    // --------------------------------------
    // Start with Scene 1 by default
    std::unique_ptr<Scene> currentScene = std::make_unique<Scene01_ClearColor>();
    currentScene->OnAttach();
    
    std::cout << "Loaded: " << currentScene->GetName() << std::endl;

    // --------------------------------------
    // GAME LOOP
    // --------------------------------------
    while (!glfwWindowShouldClose(window)) {
        
        // --- INPUT: SCENE SWITCHING ---
        
        // Key '1' -> Load Scene 01 (Colors)
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            // Only reload if we aren't already on scene 1? 
            // For now, let's just force reload for simplicity.
            currentScene = std::make_unique<Scene01_ClearColor>();
            currentScene->OnAttach();
        }

        // Key '2' -> Load Scene 02 (Keyboard Box)
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            currentScene = std::make_unique<Scene02_Input>();
            currentScene->OnAttach();
        }

        // Key '3' -> Load Scene 03 (Mouse Box)
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            currentScene = std::make_unique<Scene03_MouseInput>();
            currentScene->OnAttach();
        }

        // --- UPDATE & RENDER ---
        currentScene->OnUpdate(0.016f);
        currentScene->OnRender();

        // --- SWAP ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
