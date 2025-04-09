#pragma once

#include <GLFW/glfw3.h>
#include "../ui/BaseUI.h"
#include <vector>
#include <memory>

class Renderer {
public:
    void Init(GLFWwindow* window);
    void BeginFrame();
    void EndFrame(GLFWwindow* window);
    void Shutdown();
    void Render();
    void RenderDockSpace();

    static Renderer& GetInstance() { return *sInstance; }

private:
    std::vector<std::unique_ptr<BaseUI>> m_UIPanels;

    static Renderer* sInstance;
};