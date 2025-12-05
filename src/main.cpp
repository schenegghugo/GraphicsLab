#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

// --- SCENE HEADERS ---
#include "scenes/Scene.h" 
#include "scenes/Scene01_ClearColor.h"
#include "scenes/Scene02_Input.h"
#include "scenes/Scene03_MouseInput.h"
#include "scenes/Scene04_Optimized.h"

// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Track which scene is active to prevent reloading it every frame
int activeSceneIndex = 1; 

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
    activeSceneIndex = 1;
    
    std::cout << "Loaded: " << currentScene->GetName() << std::endl;

    // --------------------------------------
    // GAME LOOP
    // --------------------------------------
    while (!glfwWindowShouldClose(window)) {
        
        // --- INPUT: SCENE SWITCHING ---
        // We add '&& activeSceneIndex != X' to ensure we only load it ONCE per key press
        
        // Key '1' -> Load Scene 01
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && activeSceneIndex != 1) {
            currentScene = std::make_unique<Scene01_ClearColor>();
            currentScene->OnAttach();
            activeSceneIndex = 1;
            std::cout << "Switched to Scene 1" << std::endl;
        }

        // Key '2' -> Load Scene 02
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && activeSceneIndex != 2) {
            currentScene = std::make_unique<Scene02_Input>();
            currentScene->OnAttach();
            activeSceneIndex = 2;
            std::cout << "Switched to Scene 2" << std::endl;
        }

        // Key '3' -> Load Scene 03
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && activeSceneIndex != 3) {
            currentScene = std::make_unique<Scene03_MouseInput>();
            currentScene->OnAttach();
            activeSceneIndex = 3;
            std::cout << "Switched to Scene 3" << std::endl;
        }

        // Key '4' -> Load Scene 04 (FIXED TYPO HERE)
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && activeSceneIndex != 4) {
            // FIXED: Was creating Scene03, changed to Scene04_Optimized
            currentScene = std::make_unique<Scene04_Optimized>(); 
            currentScene->OnAttach();
            activeSceneIndex = 4;
            std::cout << "Switched to Scene 4" << std::endl;
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
