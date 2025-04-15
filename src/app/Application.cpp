#include "includes.h"
#include "Application.h"
#include "Renderer.h"

GLFWwindow* window = nullptr;
Renderer* renderer = nullptr;
Application* Application::sInstance = nullptr;

void Application::Init() {

    m_Running = true;
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

    // Init render
    renderer = new Renderer();
    renderer->Init(window);
}

void Application::Render() {
    // Start frame
    renderer->BeginFrame();

    renderer->Render();
    // poll events
    
    // End frame
    renderer->EndFrame(window);

    if (glfwWindowShouldClose(window)) {
        // After glfwTerminate();
        m_Running = false;
        return;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Application::Shutdown() {
    renderer->Shutdown();
}

void Application::Run() {
    Init();
    while (m_Running) {
        Render();
    }
    Shutdown();
}

void Application::Close() {
    m_Running = false;
}

Application::~Application() {
}