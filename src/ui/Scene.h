#pragma once
#include "BaseUI.h"

#include <imgui.h>

class Scene : public BaseUI {
public:
    void OnRender() override;
    void OnUpdate() override;

private:
    ImVec2 mSize;
    ImVec2 m_ViewportBounds[2];
    bool m_ViewportFocused = false, m_ViewportHovered = false;
};