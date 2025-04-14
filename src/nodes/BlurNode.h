#pragma once

#include "Node.h"

class BlurNode : public Node {
public:
    BlurNode(int id);
    ~BlurNode();

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
    void UpdateTextureFromImage();

    int kernelSize;
    int blurRadius;
    float scaleFactor;
    cv::Mat inputImage;
    cv::Mat outputImage;
};