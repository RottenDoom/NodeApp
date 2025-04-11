#include "NodeManager.h"

void NodeManager::ShowGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.5f);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    // ImGui::Begin("Toolbar", nullptr, flags);

    if (ImGui::Button("New Image")) {
        const char* path = tinyfd_openFileDialog("Open Image", "", 0, nullptr, nullptr, 0);
        if (path) {
            AddImageNode(path);
        }
    }

    // ImGui::End();
}

void NodeManager::AddImageNode(const char* path)
{
    int id = GenerateUniqueId();
    auto node = std::make_unique<ImageNode>(path);
    node->nodeId = id;
    nodes.push_back(std::move(node));
    nodeIdToIndex[id] = nodes.size() - 1;
}

void NodeManager::RenderNodes()
{
    for (auto& node : nodes) { // can't use const since I have to know about the nodes.
        node->renderUI();
    }
}

int NodeManager::GenerateUniqueId()
{
    return nextNodeId++;
}
