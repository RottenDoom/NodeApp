#pragma once
#include "BaseUI.h"
#include "nodes/ImageNode.h"
#include "nodes/NodeManager.h"
#include <imgui.h>

class Scene : public BaseUI {
public:
    void OnRender() override;
    void OnUpdate() override;
    void addNode(const char* path);

    Scene();
    ~Scene();

private:
    NodeManager manager;
    ImVec2 mSize;
    ImVec2 m_ViewportBounds[2];
    bool m_ViewportFocused = false, m_ViewportHovered = false;
};