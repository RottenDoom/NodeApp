#pragma once

#include "Node.h"

// this is a class for both brightness and contrast nodes together.
class ContrastNode : public Node {
public:
    ContrastNode(int id);
    ~ContrastNode();

    void OnRender() override;
    void OnUpdate() override;
    void RenderProperties() override;

    ImVec2 GetInputSocketPos() override;
    ImVec2 GetOutputSocketPos() override;

    void SetInputImage(const cv::Mat& img);
    cv::Mat GetOutputImage(int fromSocketIndex);
    void ExportImage(const char* path);

    bool inputConnected;

private:
    void UpdateTexture();
    void SetNodeSockets(ImVec2 nodeSize, ImVec2 nodePos);
    void InitializeSockets();

    float contrast;
    int brightness;
    float scaleFactor;
    cv::Mat inputImage;
    cv::Mat outputImage;
};