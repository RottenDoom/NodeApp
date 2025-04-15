#include "ConvolutionNode.h"
#include "NodeManager.h"

static int blurMode = 0;

ConvolutionNode::ConvolutionNode(int nodeid)
    : Node(nodeid)
{
    type = CONVULATIONAL;
    position = ImVec2(100, 100);
    kernelSize = 3; // 3 or 5
    normalization = 1.0f;

    this->nodeId = nodeid;
    InitializeSockets();
}

ConvolutionNode::~ConvolutionNode()
{
}

void ConvolutionNode::OnRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetCursorScreenPos(position);

    std::string windowName = "Convolution Node " + std::to_string(nodeId);
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
    
    // setup for the input sockets
    ImVec2 localOutputSocketPos = ImVec2(
        outputSockets[0].position.x - nodePos.x - 50,  // convert screen to local
        outputSockets[0].position.y - nodePos.y -25
    );

    ImVec2 localInputSocketPos = ImVec2(
        inputSockets[0].position.x - nodePos.x,
        inputSockets[0].position.y - nodePos.y - 25
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

    ImGui::SetCursorPos(localInputSocketPos);
    ImGui::InvisibleButton("inputSocket", socketSize);

    ImGui::EndGroup();
    ImGui::PopID();
    ImGui::End();
    
    ImGui::PopStyleVar();
}

void ConvolutionNode::OnUpdate()
{
    auto connOpt = NodeManager::GetInstance().GetInputConnection(this->nodeId, 0);
    if (!connOpt.has_value()) return;

    NodeManager::Connection conn = connOpt.value();
    auto fromNode = NodeManager::GetInstance().GetNodeById(conn.fromNodeId);
    if (!fromNode) return;

    inputImage = fromNode->GetOutputImage(conn.fromSocketIndex);
    if (inputImage.empty()) {
        std::cerr << "inputImage is empty!" << std::endl;
        return;
    }
    // apply the blur filter with kernel size and blur radius.
    cv::Mat kernelMat(kernelSize, kernelSize, CV_32F, kernel.data());
    kernelMat /= normalization;

    cv::Mat result;
    
    cv::filter2D(inputImage, result, -1, kernelMat);

    this->outputImage = result;
    this->inputConnected = true;

    UpdateTexture();
}

void ConvolutionNode::RenderProperties()
{
    ImGui::Text("Convolution Properties");

    if (!outputImage.empty() && id != 0) {
        ImGui::Text("Output Preview:");
        ImGui::Image((ImTextureID)(intptr_t)id, ImVec2(200, 200)); // You can adjust the preview size here
        ImGui::Spacing();
    } else {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "No output image available.");
        ImGui::Spacing();
        return;
    }

    ImGui::Combo("Preset", (int*)&preset, "Custom\0Identity\0Blur\0Sharpen\0Edge\0Emboss\0");
    if (ImGui::Button("Apply Preset")) {
        ApplyPreset(preset);
        OnUpdate();
        UpdateTexture();
    }

    // kernel editing
    ImGui::Text("Kernel:");
    int size = kernelSize * kernelSize;
    for (int y = 0; y < kernelSize; ++y) {
        for (int x = 0; x < kernelSize; ++x) {
            ImGui::PushID(y * kernelSize + x);
            ImGui::SetNextItemWidth(40);
            ImGui::InputFloat("", &kernel[y * kernelSize + x], 0.1f, 1.0f, "%.2f");
            ImGui::PopID();
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }

    ImGui::SliderFloat("Normalization", &normalization, 0.01f, 10.0f);
}

ImVec2 ConvolutionNode::GetInputSocketPos(int index)
{
    return inputSockets[index].position;
}

ImVec2 ConvolutionNode::GetOutputSocketPos()
{
    return outputSockets[0].position;
}

void ConvolutionNode::SetInputImage(const cv::Mat& img)
{
    inputImage = img;
}

void ConvolutionNode::UpdateTexture()
{
    if (this->inputImage.empty()) return;

    // Convert BGR to RGBA if needed
    cv::Mat rgbaImage;
    if (this->inputImage.channels() == 3) {
        cv::cvtColor(inputImage, rgbaImage, cv::COLOR_BGR2RGBA);
    } else if (this->inputImage.channels() == 1) {
        cv::cvtColor(inputImage, rgbaImage, cv::COLOR_GRAY2RGBA);
    } else {
        rgbaImage = inputImage;
    }

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

void ConvolutionNode::SetNodeSockets(ImVec2 size, ImVec2 pos)
{
    inputSockets[0].position = ImVec2(pos.x, pos.y + size.y * 0.5f);
    outputSockets[0].position = ImVec2(pos.x + size.x, pos.y + size.y * 0.5f);
}

void ConvolutionNode::InitializeSockets()
{
    inputSockets.resize(1);
    outputSockets.resize(1);
}

void ConvolutionNode::ApplyPreset(KernelPreset p)
{
    preset = p;

    if (p == KernelPreset::Identity) {
        kernel = {
            0, 0, 0,
            0, 1, 0,
            0, 0, 0
        };
        normalization = 1.0f;
    }
    else if (p == KernelPreset::Blur) {
        kernel = std::vector<float>(9, 1.0f);
        normalization = 9.0f;
    }
    else if (p == KernelPreset::Sharpen) {
        kernel = {
             0, -1,  0,
            -1,  5, -1,
             0, -1,  0
        };
        normalization = 1.0f;
    }
    else if (p == KernelPreset::Edge) {
        kernel = {
            -1, -1, -1,
            -1,  8, -1,
            -1, -1, -1
        };
        normalization = 1.0f;
    }
    else if (p == KernelPreset::Emboss) {
        kernel = {
            -2, -1,  0,
            -1,  1,  1,
             0,  1,  2
        };
        normalization = 1.0f;
    }
}

cv::Mat ConvolutionNode::GetOutputImage(int fromNodeIndex)
{
    return outputImage;
}

void ConvolutionNode::ExportImage(const char *path)
{
    if (!outputImage.empty()) {
        cv::imwrite(path, outputImage);
    }
}
