#pragma once

#include "Node.h"

// Nodes that can Sharpen or Emboss edges for higher beautification.
class ConvolutionNode : public Node {
public:
    ConvolutionNode(int id);
    ~ConvolutionNode();

    enum class KernelPreset {
        Custom,
        Identity,
        Blur,
        Sharpen,
        Edge,
        Emboss
    };

    KernelPreset preset = KernelPreset::Custom;
    std::vector<float> kernel = {
        0, 0, 0,
        0, 1, 0,
        0, 0, 0
    };

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

    int kernelSize; // 3 or 5
    float normalization;
    float scaleFactor;
    void ApplyPreset(KernelPreset p);
    cv::Mat inputImage;
    cv::Mat outputImage;
};