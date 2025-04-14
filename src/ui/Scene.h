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

    float zoom = 1.0f;
    float minZoom = 0.25f;
    float maxZoom = 2.5f;

private:
    ImVec2 mSize;
    ImVec2 m_ViewportBounds[2];
    bool m_ViewportFocused = false, m_ViewportHovered = false;

    void ShowMousePosition(const char* windowName = "viewport");
    // void ShowPreview();
};