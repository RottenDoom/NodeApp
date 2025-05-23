#pragma once

#include "Node.h"

// Blurs node for a given kernel size using Gaussian Blur
class BlurNode : public Node {
public:
    BlurNode(int id);
    ~BlurNode();

    void OnRender() override;
    void OnUpdate() override;
    void RenderProperties() override;

    ImVec2 GetInputSocketPos(int index) override;
    ImVec2 GetOutputSocketPos() override;

    void SetInputImage(const cv::Mat& img);
    cv::Mat GetOutputImage(int fromSocketIndex);
    void ExportImage(const char* path);

    bool inputConnected;

private:
    void UpdateTexture();
    void SetNodeSockets(ImVec2 nodeSize, ImVec2 nodePos);
    void InitializeSockets();

    int kernelSize;
    int blurRadius;
    float scaleFactor;
    cv::Mat inputImage;
    cv::Mat outputImage;
};