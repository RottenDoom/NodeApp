// PropertiesPanel.cpp
#include "Scene.h"

void Scene::OnRender() {
    ImGui::Begin("Viewport");

    ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
    ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    ImVec2 viewportOffset = ImGui::GetWindowPos();
    m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();
    

    // addNode("../assets/image.png"); // outside the build folder and then into assets
    // ImGui::SetCursorScreenPos(ImVec2(viewportOffset.x + 10, viewportOffset.y + 10));
    // ImGui::ShowDemoWindow();

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImVec2 center = ImVec2(viewportMinRegion.x + 10, viewportMinRegion.y + 10);
    ImGui::SetCursorPos(center);
    ImGui::SetNextWindowBgAlpha(0.5f);

    if (ImGui::Button("New Image")) {
        const char* path = tinyfd_openFileDialog("Open Image", "", 0, nullptr, nullptr, 0);
        if (path) {
            manager.AddImageNode(path);
        }
    }

    manager.RenderNodes();

    enum class SceneState
    {
        Edit = 0, Play = 1, Simulate = 2
    };
    SceneState m_SceneState = SceneState::Edit;
    
    ImGui::End();
}

void Scene::OnUpdate() {

}

void Scene::addNode(const char* path){
    ImageNode node(path);
    node.OnRender();
}

Scene::Scene()
{
}

Scene::~Scene()
{
}
