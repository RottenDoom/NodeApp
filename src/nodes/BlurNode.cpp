#include "BlurNode.h"
#include "NodeManager.h"

static int blurMode = 0;

BlurNode::BlurNode(int nodeid)
    : Node(nodeid)
{
    type = BLUR;
    position = ImVec2(100, 100);
    blurRadius = 5;

    this->nodeId = nodeid;
    InitializeSockets();
}

BlurNode::~BlurNode()
{
}

void BlurNode::OnRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetCursorScreenPos(position);

    std::string windowName = "Blur Node " + std::to_string(nodeId);
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

void BlurNode::OnUpdate()
{
    auto connOpt = NodeManager::GetInstance().GetInputConnection(this->nodeId, 0);
    if (!connOpt.has_value()) return;

    NodeManager::Connection conn = connOpt.value();
    auto fromNode = NodeManager::GetInstance().GetNodeById(conn.fromNodeId);
    if (!fromNode) return;

    inputImage = fromNode->GetOutputImage(conn.fromSocketIndex);
    if (inputImage.empty()) return;

    // Apply a blur filter
    cv::Mat blurred;
    cv::GaussianBlur(inputImage, blurred, cv::Size(kernelSize, kernelSize), blurRadius);

    // Store result in this node's image
    this->outputImage = blurred;
    this->inputConnected = true;

    // If you're also managing GPU texture, update the OpenGL texture from the image
    UpdateTexture();
}

void BlurNode::RenderProperties()
{
    ImGui::Text("Blur Node Properties");

    if (!outputImage.empty() && id != 0) {
        ImGui::Text("Output Preview:");
        ImGui::Image((ImTextureID)(intptr_t)id, ImVec2(160, 90)); // You can adjust the preview size here
        ImGui::Spacing();
    } else {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "No output image available.");
        ImGui::Spacing();
        return;
    }

    ImGui::Text("Blur Properties:");
    ImGui::SliderInt("Radius", &blurRadius, 1, 20);
    kernelSize = 2 * blurRadius + 1;

    const char* blurModes[] = { "Uniform", "Horizontal", "Vertical" };
    ImGui::Combo("Mode", &blurMode, blurModes, IM_ARRAYSIZE(blurModes));

    if (blurMode == 0) {
        cv::GaussianBlur(inputImage, outputImage, cv::Size(kernelSize, kernelSize), blurRadius);
    }
    else if (blurMode == 1) {
        cv::GaussianBlur(inputImage, outputImage, cv::Size(kernelSize, 1), blurRadius);
    }
    else{
        cv::GaussianBlur(inputImage, outputImage, cv::Size(1, kernelSize), blurRadius);
    }
    // Generate Gaussian kernel using OpenCV
    cv::Mat kernelX = cv::getGaussianKernel(kernelSize, blurRadius);
    cv::Mat kernel2D = kernelX * kernelX.t();

    // Normalize for visualization
    cv::normalize(kernel2D, kernel2D, 0, 1, cv::NORM_MINMAX);

    // Display kernel
    ImGui::Text("Kernel:");
    for (int i = 0; i < kernel2D.rows; ++i) {
        for (int j = 0; j < kernel2D.cols; ++j) {
            ImGui::Text("%.2f ", kernel2D.at<double>(i, j));
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }
}

ImVec2 BlurNode::GetInputSocketPos()
{
    return inputSockets[0].position;
}

ImVec2 BlurNode::GetOutputSocketPos()
{
    return outputSockets[0].position;
}

void BlurNode::SetInputImage(const cv::Mat& img)
{
    inputImage = img;
}

void BlurNode::UpdateTexture()
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

void BlurNode::SetNodeSockets(ImVec2 size, ImVec2 pos)
{
    inputSockets[0].position = ImVec2(pos.x, pos.y + size.y * 0.5f);
    inputSockets[1].position = ImVec2(pos.x, pos.y + size.y * 0.5f);
    outputSockets[0].position = ImVec2(pos.x + size.x, pos.y + size.y * 0.5f);
}

void BlurNode::InitializeSockets()
{
    inputSockets.resize(1);
    outputSockets.resize(1);
}

void BlurNode::UpdateTextureFromImage()
{
}

cv::Mat BlurNode::GetOutputImage(int fromNodeId)
{
    return outputImage;
    // std::shared_ptr<ImageNode> node = std::static_pointer_cast<ImageNode>(NodeManager().GetInstance().nodeMap[fromNodeId]);
    // return node->texture.originalImage;
}

void BlurNode::ExportImage(const char *path)
{
    if (!outputImage.empty()) {
        cv::imwrite(path, outputImage);
    }
}
