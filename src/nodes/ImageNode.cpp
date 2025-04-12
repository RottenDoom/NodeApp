#include "ImageNode.h"
#include "Texture.h"

bool connectionMade = false;

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
    
}

ImVec2 ImageNode::GetInputSocketPos()
{
    return inputSocket.position;
}

ImVec2 ImageNode::GetOutputSocketPos()
{
    return outputSocket.position;
}

void ImageNode::renderToFramebuffer()
{
}

void ImageNode::InitializeSockets()
{
    inputSocket.nodeId = nodeId;
    outputSocket.nodeId = nodeId;
    inputSocket.position = ImVec2(0, 0);
    outputSocket.position = ImVec2(0, 0);
}

void ImageNode::SetNodeSockets(ImVec2 size, ImVec2 pos)
{
    inputSocket.position = ImVec2(pos.x, pos.y + size.y * 0.5f);   
    outputSocket.position = ImVec2(pos.x + size.x, pos.y + size.y * 0.5f);
}

void ImageNode::OnRender() 
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
    ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    ImVec2 viewportOffset = ImGui::GetWindowPos();

    m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
    
    if (!loaded) {
        ImGui::Text("Image not loaded.");
        return;
    }

    std::string windowName = "Node " + std::to_string(nodeId);
    ImGui::Begin(
        windowName.c_str(),
        nullptr,
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse
    );

    ImGui::PushID(nodeId);

    ImVec2 nodeSize = ImGui::GetWindowContentRegionMax();
    ImVec2 nodePos = ImGui::GetWindowPos();
    
    if (texture.textureid != 0) {
        ImGui::Image((ImTextureID)(intptr_t)texture.textureid, nodeSize);
    } else {
        ImGui::Text("Texture is not loaded.");
    }

    SetNodeSockets(nodeSize, nodePos);
    
    ImGui::BeginGroup();

    ImVec2 socketSize = ImVec2(50, 50);
    
    // setup for the input sockets
    ImVec2 localOutputSocketPos = ImVec2(
        outputSocket.position.x - nodePos.x - 50,  // convert screen to local
        outputSocket.position.y - nodePos.y -25
    );

    ImVec2 localInputSocketPos = ImVec2(
        inputSocket.position.x - nodePos.x,
        inputSocket.position.y - nodePos.y - 25
    );

    ImGui::SetCursorPos(localOutputSocketPos);
    ImGui::InvisibleButton("outputSocket", socketSize);

    ImDrawList* fg = ImGui::GetForegroundDrawList();
    fg->AddCircleFilled(outputSocket.position, 6.0f, IM_COL32(0, 255, 0, 255));
    ImVec2 startPos;
    ImVec2 endPos;
    
    if (ImGui::IsItemActive()) {
        NodeManager::GetInstance().StartConnectionDrag(this->nodeId, NodeManager::SocketType::Output, outputSocket.position);
    }

    ImGui::SetCursorPos(localInputSocketPos);
    ImGui::Button("inputSocket", socketSize);

    bool hovered = ImGui::IsItemHovered();
    bool released = ImGui::IsMouseReleased(0);
    if (hovered && released) {
        NodeManager::GetInstance().TryCreateConnection(this->nodeId, NodeManager::SocketType::Input);
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