#pragma once

#include <GLFW/glfw3.h>

class Renderer {
public:
    void Init(GLFWwindow* window);
    void BeginFrame();
    void EndFrame(GLFWwindow* window);
    void Shutdown();
    void Render();
    void RenderDockSpace();
};