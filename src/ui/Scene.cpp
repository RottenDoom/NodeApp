// PropertiesPanel.cpp
#include "Scene.h"

void Scene::OnRender() {
    ImGui::Begin("Viewport");

    ShowMousePosition();

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
            NodeManager::GetInstance().AddImageNode(path);
        }
    }

    NodeManager::GetInstance().RenderNodes();

    enum class SceneState
    {
        Edit = 0, Play = 1, Simulate = 2
    };
    SceneState m_SceneState = SceneState::Edit;
    
    ImGui::End();
}

void Scene::OnUpdate() {

}

Scene::Scene()
{
}

Scene::~Scene()
{
}


void Scene::ShowMousePosition(const char *windowName)
{
    ImVec2 windowPos = ImGui::GetWindowPos();         // Top-left in screen space
    ImVec2 mouseScreen = ImGui::GetMousePos();        // Mouse in screen space
    ImVec2 relative = mouseScreen - windowPos;        // Relative to this window

    // Move to top-right corner of the window
    ImVec2 textSize = ImGui::CalcTextSize("X:0000 Y:0000");
    ImVec2 cornerPos = ImVec2(ImGui::GetWindowSize().x - textSize.x - 10, 30);

    ImGui::SetCursorPos(cornerPos);
    ImGui::Text("X: %.0f Y: %.0f", relative.x, relative.y);
}
