#include <glad/glad.h>

#include "Application.h"
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include "Renderer.h"

// todo I am not going to add imgui code here bruh i am gonna make a imgui layer like cherno to add the layers that I am gonna add tomorow


GLFWwindow* window = nullptr;
Renderer* renderer = nullptr;

void Application::Init() {
    // Init GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW.\n";
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window = glfwCreateWindow(1280, 720, "NodeApp", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Init OpenGL loader (GLAD)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD.\n";
        return;
    }

    // Init rendere
    renderer = new Renderer();
    renderer->Init(window);
}

void Application::Render() {
    // Start frame
    renderer->BeginFrame();

    renderer->Render();

    // End frame
    renderer->EndFrame(window);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Application::Shutdown() {
    renderer->Shutdown();
}

void Application::Run() {
    Init();
    while (!glfwWindowShouldClose(window)) {
        Render();
    }
    Shutdown();
}