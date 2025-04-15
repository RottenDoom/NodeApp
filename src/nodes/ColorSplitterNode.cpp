#include "ColorSplitterNode.h"
#include "NodeManager.h"

static int blurMode = 0;

ColorSplitterNode::ColorSplitterNode(int nodeid)
  : Node(nodeid),  // name + default size
    inputConnected(false)
{
    type = COLOR_SPLITTER;
    position = ImVec2(100, 100);
    mode = OutputMode::RGB;
    selectedChannel = 0;
    InitializeSockets();
}

ColorSplitterNode::~ColorSplitterNode()
{
}

void ColorSplitterNode::OnRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetCursorScreenPos(position);

    std::string windowName = "Color Splitter Node " + std::to_string(nodeId);
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

void ColorSplitterNode::OnUpdate()
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

    // Ensure image has 3 channels
    if (inputImage.channels() != 3) {
        if (inputImage.channels() == 4)
            cv::cvtColor(inputImage, inputImage, cv::COLOR_BGRA2BGR);
        else if (inputImage.channels() == 1)
            cv::cvtColor(inputImage, inputImage, cv::COLOR_GRAY2BGR);
    }

    switch (mode) {
        case OutputMode::RGB:       ProcessRGBMode(inputImage);       break;
        case OutputMode::Grayscale: ProcessGrayscaleMode(inputImage); break;
        case OutputMode::HSV:       ProcessHSVMode(inputImage);       break;
    }

    this->outputImage = outputChannels[selectedChannel];

    inputConnected = true;
    UpdateTexture();
}

void ColorSplitterNode::RenderProperties()
{
    ImGui::Text("Color Splitter Preview");

    if (!outputImage.empty() && id != 0) {
        ImGui::Text("Output Preview:");
        ImGui::Image((ImTextureID)(intptr_t)id, ImVec2(200, 200)); // You can adjust the preview size here
        ImGui::Spacing();
    } else {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "No output image available.");
        ImGui::Spacing();
        return;
    }

    ImGui::Text("Mode:");
    const char* modes[] = { "RGB", "Grayscale", "HSV" };
    int current = static_cast<int>(mode);
    if (ImGui::Combo("Color Mode", &current, modes, IM_ARRAYSIZE(modes))) {
        mode = static_cast<OutputMode>(current);
        OnUpdate();
        UpdateTexture();
    }

    ImGui::Separator();

    ImGui::Text("Preview Channel:");
    const char* channels[] = { "Channel 0", "Channel 1", "Channel 2" };
    if (ImGui::Combo("Display", &selectedChannel, channels, IM_ARRAYSIZE(channels))) {
        this->outputImage = outputChannels[selectedChannel];
        UpdateTexture();
    }

    ImGui::Spacing();
    ImGui::Text("Output mapping:");
    if (mode == OutputMode::RGB) {
        ImGui::BulletText("0 = Red");
        ImGui::BulletText("1 = Green");
        ImGui::BulletText("2 = Blue");
    } else if (mode == OutputMode::Grayscale) {
        ImGui::BulletText("0 = Red intensity");
        ImGui::BulletText("1 = Green intensity");
        ImGui::BulletText("2 = Blue intensity");
    } else if (mode == OutputMode::HSV) {
        ImGui::BulletText("0 = Hue");
        ImGui::BulletText("1 = Saturation");
        ImGui::BulletText("2 = Value");
    }
}

ImVec2 ColorSplitterNode::GetInputSocketPos(int index)
{
    return inputSockets[0].position;
}

ImVec2 ColorSplitterNode::GetOutputSocketPos()
{
    return outputSockets[0].position;
}

void ColorSplitterNode::SetInputImage(const cv::Mat& img)
{
    inputImage = img;
}

void ColorSplitterNode::ProcessRGBMode(const cv::Mat &input)
{
    // Split into BGR channels.
    std::vector<cv::Mat> channels;
    cv::split(input, channels);  // OpenCV stores in BGR order.

    if(channels.size() != 3)
        return;

    // Create a black channel of the same size.
    cv::Mat zero = cv::Mat::zeros(channels[0].size(), channels[0].type());

    // Build an image for each output:
    // We want the isolated color channel displayed in its corresponding position.
    // For display purposes, we merge the isolated channel with zeros.
    // Note: For "RGB mode", we assume output order: Red, Green, Blue.
    // Since channels order from cv::split is B, G, R,
    // we use:
    std::vector<cv::Mat> redChannels = { zero, zero, channels[2] };
    cv::merge(redChannels, outputChannels[0]);
    
    std::vector<cv::Mat> greenChannels = { zero, channels[1], zero };
    cv::merge(greenChannels, outputChannels[1]);

    std::vector<cv::Mat> blueChannels = { channels[0], zero, zero };
    cv::merge(blueChannels, outputChannels[2]);
}

void ColorSplitterNode::ProcessGrayscaleMode(const cv::Mat &input)
{
    // Split channels.
    std::vector<cv::Mat> channels;
    cv::split(input, channels);  // B, G, R

    if(channels.size() != 3)
        return;

    // In grayscale mode, simply output the channel as is.
    // For preview, we convert to 3 channels (if needed).
    cv::Mat redGray, greenGray, blueGray;
    cv::cvtColor(channels[2], outputChannels[0], cv::COLOR_GRAY2BGR);   // Red channel from 3rd element.
    cv::cvtColor(channels[1], outputChannels[1], cv::COLOR_GRAY2BGR); // Green channel.
    cv::cvtColor(channels[0], outputChannels[2], cv::COLOR_GRAY2BGR);  // Blue channel.
}

void ColorSplitterNode::ProcessHSVMode(const cv::Mat &input)
{
    cv::Mat hsv;
    cv::cvtColor(input, hsv, cv::COLOR_BGR2HSV);

    std::vector<cv::Mat> ch;
    cv::split(hsv, ch); // H, S, V

    for (int i = 0; i < 3; ++i)
        cv::cvtColor(ch[i], outputChannels[i], cv::COLOR_GRAY2BGR);
}

void ColorSplitterNode::UpdateTexture()
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

void ColorSplitterNode::SetNodeSockets(ImVec2 size, ImVec2 pos)
{
    inputSockets[0].position = ImVec2(pos.x, pos.y + size.y * 0.5f);
    outputSockets[0].position = ImVec2(pos.x + size.x, pos.y + size.y * 0.5f);
}

void ColorSplitterNode::InitializeSockets()
{
    inputSockets.resize(1);
    outputSockets.resize(1);
}

cv::Mat ColorSplitterNode::GetOutputImage(int fromNodeIndex)
{
    return outputImage;
}

void ColorSplitterNode::ExportImage(const char *path)
{
    if (!outputImage.empty()) {
        cv::imwrite(path, outputImage);
    }
}
