#include "ImageNode.h"
#include "Texture.h"

ImageNode::ImageNode(const char* imagePath)
    : texture(&id, &w, &h, imagePath)
    // framebuffer(texture.width, texture.height)
{
    loaded = texture.textureid != 0;
    if (!loaded) {
        std::cerr << "Failed to load image\n";
        return;
    }

    this->nodeId = 0;
    InitializeSockets();

    // framebuffer.create(texture.width, texture.height);
    // renderToFramebuffer();
}

ImageNode::~ImageNode()
{
    // framebuffer.destroy();
}

ImVec2 ImageNode::GetInputSocketPos(int index)
{
    return ImVec2();
}

ImVec2 ImageNode::GetOutputSocketPos(int index)
{
    return ImVec2();
}

void ImageNode::renderToFramebuffer()
{
}

void ImageNode::InitializeSockets()
{    
    inputSocket.position = ImVec2(0, 0);
    inputSocket.id = nodeId;

    outputSocket.position = ImVec2(0, 0);
    outputSocket.id = nodeId + 1;
}

void ImageNode::SetNodeSockets(ImVec2 size, ImVec2 pos)
{
    inputSocket.position = ImVec2(pos.x, pos.y + size.y * 0.5f);   
    outputSocket.position = ImVec2(pos.x + size.x, pos.y + size.y * 0.5f);
}

void ImageNode::OnRender() 
{
    if (!loaded) {
        ImGui::Text("Image not loaded.");
        return;
    }

    ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
    ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    ImVec2 viewportOffset = ImGui::GetWindowPos();

    m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

    std::string windowName = "Node " + std::to_string(nodeId);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::Begin(
        windowName.c_str(),
        nullptr,
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse
    );

    ImGui::PushID(nodeId);
    ImVec2 nodeSize = ImGui::GetWindowContentRegionMax();
    ImVec2 nodePos = ImGui::GetWindowContentRegionMin();

    // std::cout << nodeSize.x << ", " << nodeSize.y << "\n";

    // Draw background box (optional)
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    if (texture.textureid != 0) {
        ImGui::Image((ImTextureID)(intptr_t)texture.textureid, nodeSize);
    } else {
        ImGui::Text("Texture is not loaded.");
    }
    
    SetNodeSockets(nodeSize, nodePos);
    
    ImGui::SetCursorPos(nodePos);
    ImGui::BeginGroup();
    // todo
    ImVec2 socketSize = ImVec2(10, 10);
    
    ImGui::SetCursorScreenPos(outputSocket.position);
    ImGui::Button("socket", socketSize);
    ImDrawList* fg = ImGui::GetForegroundDrawList();
    fg->AddCircleFilled(outputSocket.position, 6.0f, IM_COL32(0, 255, 0, 255));
    // fg->AddRect()
    
    if (ImGui::IsItemActive()) {
        ImVec2 startPos = outputSocket.position + socketSize * 0.5f;
        ImVec2 endPos = ImGui::GetIO().MousePos;
        
        ImVec2 cp1 = startPos + ImVec2(50, 0); // control point 1
        ImVec2 cp2 = endPos + ImVec2(-50, 0);  // control point 2
        
        ImGui::GetForegroundDrawList()->AddBezierCubic(
            startPos, cp1, cp2, endPos,
            IM_COL32(200, 200, 100, 255), 3.0f
        );
    }
    
    ImGui::EndGroup();
    
    ImGui::PopID();
    
    
    ImGui::End();
    
    ImGui::PopStyleVar();

    
}
void ImageNode::OnUpdate()
{
    
}
// drawList->AddRectFilled(nodePos, nodePos + nodeSize, IM_COL32(50, 50, 50, 255), 6.0f);

// if (nodePos.x < viewportMinRegion.x) nodePos.x = viewportMinRegion.x;
// if (nodePos.y < viewportMinRegion.y) nodePos.y = viewportMinRegion.y;
// if (nodePos.x + windowSize.x > viewportMaxRegion.x) nodePos.x = viewportMaxRegion.x - windowSize.x;
// if (nodePos.y + windowSize.y > viewportMaxRegion.y) nodePos.y = viewportMaxRegion.y - windowSize.y;

// ImGui::SetWindowPos(nodePos);