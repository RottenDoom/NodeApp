#include "BlendNode.h"
#include "NodeManager.h"

static int blurMode = 0;

BlendNode::BlendNode(int nodeid)
    : Node(nodeid), alpha(0.5f), blendMode(BlendMode::Alpha)
{
    type = BLEND_NODE;
    position = ImVec2(100, 100);
    inputsConnected[0] = inputsConnected[1] = false;

    this->nodeId = nodeid;
    InitializeSockets();
}

BlendNode::~BlendNode()
{
}

void BlendNode::OnRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetCursorScreenPos(position);

    std::string windowName = "Blend Node " + std::to_string(nodeId);
    ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin(
        windowName.c_str(),
        nullptr,
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse
    );

    ImGui::PushID(nodeId);

    ImVec2 nodeSize = ImGui::GetWindowContentRegionMax();
    ImVec2 nodePos = ImGui::GetWindowPos();
    ImVec2 contentSize = ImGui::GetContentRegionAvail();

    if (id && inputConnected) {
        scaleFactor = std::min(200.0f / outputImage.cols, 200.0f / outputImage.rows);
        ImGui::Image((ImTextureID)(intptr_t)id, contentSize);

        if (ImGui::BeginPopupContextItem("ImageRightClick")) {
            if (ImGui::MenuItem("Save Image...")) {
                const char* filters[] = { "*.png", "*.jpg", "*.bmp" };
                const char* file = tinyfd_saveFileDialog(
                    "Save Processed Image",
                    "output.png",
                    3,
                    filters,
                    "Image files"
                );
    
                if (file) {
                    ExportImage(file);
                }
            }
            ImGui::EndPopup();
        }
    } else {
        ImGui::Dummy(ImVec2(160, 90));
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
    
    for (int i = 0; i < 2; ++i) {
        ImVec2 localPos = ImVec2(inputSockets[i].position.x - nodePos.x, inputSockets[i].position.y - nodePos.y - 25);
        ImGui::SetCursorPos(localPos);
        ImGui::InvisibleButton(("inputSocket" + std::to_string(i)).c_str(), socketSize);

        ImDrawList* fg = ImGui::GetForegroundDrawList();
        fg->AddCircleFilled(inputSockets[i].position, 6.0f, IM_COL32(0, 255, 0, 255));
        if (ImGui::IsItemActive()) {
            NodeManager::GetInstance().StartConnectionDrag(nodeId, NodeManager::SocketType::Input, inputSockets[i].position, i);
        }
    }

    // setup for the input sockets
    ImVec2 localOutputSocketPos = ImVec2(
        outputSockets[0].position.x - nodePos.x - 50,  // convert screen to local
        outputSockets[0].position.y - nodePos.y -25
    );

    ImGui::SetCursorPos(localOutputSocketPos);
    ImGui::InvisibleButton("outputSocket", socketSize);

    ImDrawList* fg = ImGui::GetForegroundDrawList();
    fg->AddCircleFilled(outputSockets[0].position, 6.0f, IM_COL32(0, 255, 0, 255));
    ImVec2 startPos;
    ImVec2 endPos;
    
    if (ImGui::IsItemActive()) {
        NodeManager::GetInstance().StartConnectionDrag(this->nodeId, NodeManager::SocketType::Output, outputSockets[0].position, 0);
    }

    ImGui::EndGroup();
    ImGui::PopID();
    ImGui::End();
    
    ImGui::PopStyleVar();
}

void BlendNode::OnUpdate()
{
    inputImageA.release();
    inputImageB.release();
    inputsConnected[0] = inputsConnected[1] = false;

    for (int i = 0; i < 2; ++i) {
        auto connOpt = NodeManager::GetInstance().GetInputConnection(nodeId, i);
        if (!connOpt.has_value()) continue;

        auto fromNode = NodeManager::GetInstance().GetNodeById(connOpt->fromNodeId);
        if (!fromNode) continue;

        cv::Mat img = fromNode->GetOutputImage(connOpt->fromSocketIndex);
        if (!img.empty()) {
            if (i == 0) inputImageA = img;
            else inputImageB = img;
            inputsConnected[i] = true;
        }
    }


    if (inputsConnected[0] && inputsConnected[1]) {
        ProcessBlend();
        UpdateTexture();
    }
}

void BlendNode::RenderProperties()
{
    ImGui::Text("Blend Node Properties");

    if (!outputImage.empty() && id != 0) {
        ImGui::Text("Output Preview:");
        ImGui::Image((ImTextureID)(intptr_t)id, ImVec2(200, 200)); // You can adjust the preview size here
        ImGui::Spacing();
    } else {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "No output image available.");
        ImGui::Spacing();
        return;
    }

    const char* modes[] = { "Add", "Multiply", "Alpha Blend" };
    int current = static_cast<int>(blendMode);
    if (ImGui::Combo("Blend Mode", &current, modes, IM_ARRAYSIZE(modes))) {
        blendMode = static_cast<BlendMode>(current);
    }

    if (blendMode == BlendMode::Alpha) {
        ImGui::SliderFloat("Alpha", &alpha, 0.0f, 1.0f);
    }
}

ImVec2 BlendNode::GetInputSocketPos(int index)
{
    return inputSockets[index].position;
}

ImVec2 BlendNode::GetOutputSocketPos()
{
    return outputSockets[0].position;
}

void BlendNode::SetInputImage(const cv::Mat& img)
{
    // does nothing
}

void BlendNode::ProcessBlend() {
    if (inputImageA.empty() || inputImageB.empty()) return;

    cv::Mat a, b;
    cv::resize(inputImageA, a, inputImageB.size());

    switch (blendMode) {
        case BlendMode::Add:
            cv::add(a, inputImageB, outputImage);
            break;
        case BlendMode::Multiply:
            cv::multiply(a, inputImageB, outputImage, 1.0 / 255);
            break;
        case BlendMode::Alpha:
            cv::addWeighted(a, alpha, inputImageB, 1.0 - alpha, 0.0, outputImage);
            break;
    }
}

void BlendNode::UpdateTexture()
{
    if (outputImage.empty()) return;

    cv::Mat rgbaImage;
    cv::cvtColor(outputImage, rgbaImage, cv::COLOR_BGR2RGBA);

    // Generate texture if it doesn't exist
    if (this->id == 0) {
        glGenTextures(1, &this->id);
    }

    glBindTexture(GL_TEXTURE_2D, this->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rgbaImage.cols, rgbaImage.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaImage.data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void BlendNode::SetNodeSockets(ImVec2 size, ImVec2 pos)
{
    if (!inputSockets.empty()) {
        float spacing = size.y / (inputSockets.size() + 1);
        for (int i = 0; i < inputSockets.size(); ++i) {
            inputSockets[i].position = ImVec2(pos.x, pos.y + spacing * (i + 1));
        }
    }
    float spacing = size.y / (outputSockets.size() + 1);
    for (int i = 0; i < outputSockets.size(); ++i) {
        outputSockets[i].position = ImVec2(pos.x + size.x, pos.y + spacing * (i + 1));
    }
}

void BlendNode::InitializeSockets()
{
    inputSockets.resize(2);
    outputSockets.resize(1);
}

cv::Mat BlendNode::GetOutputImage(int fromNodeIndex)
{
    return outputImage;
}

void BlendNode::ExportImage(const char *path)
{
    if (!outputImage.empty()) {
        cv::imwrite(path, outputImage);
    }
}
