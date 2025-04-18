#pragma once

#include "Node.h"

// threshold node for applying binary operations onto the nodes. Otsu and Adaptive modes available
class ThresholdNode : public Node {
public:
    ThresholdNode(int id);
    ~ThresholdNode();

    enum class ThresholdMethod {
        Binary,
        Otsu,
        AdaptiveMean,
        AdaptiveGaussian
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

    float threshValue;
    int blockSize;
    float C;
    ThresholdMethod thresholdMethod;
    bool useOtsu = false;
    float histogramDataArray[256];
    cv::Mat hist;
    float scaleFactor;
    cv::Mat inputImage;
    cv::Mat outputImage;
};