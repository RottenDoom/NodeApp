#include "ImageNode.h"
#include "Texture.h"

bool connectionMade = false;

ImageNode::ImageNode(const int nodeid, const char* imagePath)
    : Node(nodeid),
      texture(&id, &w, &h, imagePath)
    // framebuffer(texture.width, texture.height)
{
    loaded = texture.textureid != 0;
    if (!loaded) {
        std::cerr << "Failed to load image\n";
        return;
    }

    this->type = Node::Type::IMAGE;

    this->nodeId = nodeid;
    InitializeSockets();

    // framebuffer.create(texture.width, texture.height);
    // renderToFramebuffer();
}

ImageNode::~ImageNode()
{
    
}

// to do add this to properties class
void ImageNode::RenderProperties()
{
    ImGui::Text("Node ID: %d", nodeId);
    ImGui::Text("Dimensions: %d x %d", texture.originalImage.cols, texture.originalImage.rows);
    ImGui::Text("Format: %s", "RGBA");
    ImGui::Text("File Size: %.2f MB", fileSize / 1024.0f * 1024.0f);

    ImGui::Separator();
    ImGui::Text("Here goes the preview Image.");
    
    // Check node type
    if (this->GetType() == Node::Type::IMAGE) {
        auto imageNode = std::static_pointer_cast<ImageNode>(NodeManager::GetInstance().nodeMap[NodeManager::GetInstance().selectedNodeId]);

        // Preview
        float previewWidth = 200.0f;
        float aspectRatio = static_cast<float>(imageNode->texture.originalImage.rows) / imageNode->texture.originalImage.cols;
        float previewHeight = previewWidth * aspectRatio;

        ImGui::Text("Preview:");
        ImGui::Image(
            (ImTextureID)(intptr_t)imageNode->texture.textureid,
            ImVec2(previewWidth, previewHeight),
            ImVec2(0, 0), ImVec2(1, 1)
        );
    }
}

const int ImageNode::GetId() const
{
    return this->nodeId;
}

ImVec2 ImageNode::GetInputSocketPos(int index)
{
    return inputSockets[index].position;
}

ImVec2 ImageNode::GetOutputSocketPos()
{
    return outputSockets[0].position;
}

void ImageNode::renderToFramebuffer()
{
}

cv::Mat ImageNode::GetOutputImage(int fromNodeIndex)
{
    return outputImage;
}

void ImageNode::InitializeSockets()
{
    inputSockets.resize(0);
    outputSockets.resize(1);
}

void ImageNode::SetNodeSockets(ImVec2 size, ImVec2 pos)
{
    if (!inputSockets.empty()) {
        inputSockets[0].position = ImVec2(pos.x, pos.y + size.y * 0.5f);
    }
    float spacing = size.y / (outputSockets.size() + 1);
    for (int i = 0; i < outputSockets.size(); ++i) {
        outputSockets[i].position = ImVec2(pos.x + size.x, pos.y + spacing * (i + 1));
    }
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

    ImVec2 mousePos = ImGui::GetMousePos();
    bool isHovered = ImGui::IsMouseHoveringRect(nodePos, nodePos + nodeSize);
    bool isClicked = isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    if (isClicked) {
        NodeManager::GetInstance().SetSelectedNode(this->nodeId);
    }

    SetNodeSockets(nodeSize, nodePos);
    
    ImGui::BeginGroup();

    ImVec2 socketSize = ImVec2(50, 50);
    
    // setup for the input sockets
    for (int i = 0; i < outputSockets.size(); i++) {
        ImVec2 localOutputSocketPos = ImVec2(
            outputSockets[i].position.x - nodePos.x - 50,  // convert screen to local
            outputSockets[i].position.y - nodePos.y -25
        );

        ImGui::SetCursorPos(localOutputSocketPos);
        ImGui::InvisibleButton(("outputSocket" + std::to_string(i)).c_str(), ImVec2(50, 50));
        
        ImDrawList* fg = ImGui::GetForegroundDrawList();
        fg->AddCircleFilled(outputSockets[i].position, 6.0f, IM_COL32(0, 255, 0, 255));
        
        if (ImGui::IsItemActive()) {
            for (int i = 0; i< outputSockets.size(); i++) {
                NodeManager::GetInstance().StartConnectionDrag(this->nodeId, NodeManager::SocketType::Output, outputSockets[i].position, i);
            }
        }
    }
    
    if (!inputSockets.empty()) {
        ImVec2 localInputSocketPos = ImVec2(
            inputSockets[0].position.x - nodePos.x,
            inputSockets[0].position.y - nodePos.y - 25
        );

        ImGui::SetCursorPos(localInputSocketPos);
        ImGui::InvisibleButton("inputSocket", socketSize);
    }
    
    ImGui::EndGroup();
    ImGui::PopID();
    ImGui::End();
    
    ImGui::PopStyleVar();
}

void ImageNode::OnUpdate()
{
    if (outputImage.empty()) {
        outputImage = texture.originalImage.clone(); // clone if you expect downstream processing to modify
    }
}



// TODO == clamp nodes inside only viewport.
// drawList->AddRectFilled(nodePos, nodePos + nodeSize, IM_COL32(50, 50, 50, 255), 6.0f);

// if (nodePos.x < viewportMinRegion.x) nodePos.x = viewportMinRegion.x;
// if (nodePos.y < viewportMinRegion.y) nodePos.y = viewportMinRegion.y;
// if (nodePos.x + windowSize.x > viewportMaxRegion.x) nodePos.x = viewportMaxRegion.x - windowSize.x;
// if (nodePos.y + windowSize.y > viewportMaxRegion.y) nodePos.y = viewportMaxRegion.y - windowSize.y;

// ImGui::SetWindowPos(nodePos);