#pragma once

#include "Node.h"

// Detects edge and applies Sobel or Canny method to emboss at edges.
class EdgeDetectionNode : public Node {
public:
    EdgeDetectionNode(int id);
    ~EdgeDetectionNode();

    enum class Method {
        Sobel,
        Canny
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

    Method method;
    int sobelKernelSize;
    int cannyThreshold1;
    int cannyThreshold2;
    bool overlayEdges;

    cv::Mat inputImage;
    cv::Mat edgeImage;
    cv::Mat outputImage;  // potentially overlayed

    float scaleFactor;
};
