// PropertiesPanel.cpp
#include "Scene.h"

void Scene::OnRender() {
    ImGui::Begin("Viewport");

    auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewportOffset = ImGui::GetWindowPos();
    m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();

    enum class SceneState
    {
        Edit = 0, Play = 1, Simulate = 2
    };
    SceneState m_SceneState = SceneState::Edit;
    
    ImGui::End();
}

void Scene::OnUpdate() {

}