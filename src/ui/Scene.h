#pragma once
#include "BaseUI.h"
#include "nodes/ImageNode.h"
#include <imgui.h>

class Scene : public BaseUI {
public:
    void OnRender() override;
    void OnUpdate() override;

    Scene();
    ~Scene();

private:
    ImVec2 mSize;
    ImVec2 m_ViewportBounds[2];
    bool m_ViewportFocused = false, m_ViewportHovered = false;

    void ShowMousePosition(const char* windowName = "viewport");
    // void ShowPreview();
};