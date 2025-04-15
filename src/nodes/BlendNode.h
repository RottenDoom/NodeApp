#pragma once

#include "Node.h"

// Blends two nodes using alpha values.
class BlendNode : public Node {
public:
    BlendNode(int id);
    ~BlendNode();

    enum class BlendMode {
        Add,
        Multiply,
        Alpha
    };

    void OnRender() override;
    void OnUpdate() override;
    void RenderProperties() override;

    ImVec2 GetInputSocketPos(int index) override;
    ImVec2 GetOutputSocketPos() override;

    void SetInputImage(const cv::Mat& img);
    cv::Mat GetOutputImage(int fromSocketIndex);
    void ExportImage(const char* path);

    bool inputConnected[2];

private:
    void ProcessBlend();
    void UpdateTexture();
    void SetNodeSockets(ImVec2 nodeSize, ImVec2 nodePos);
    void InitializeSockets();

    cv::Mat inputImageA;
    cv::Mat inputImageB;
    cv::Mat outputImage;

    GLuint id = 0;
    float alpha = 0.5f;
    BlendMode blendMode;
    bool inputsConnected[2];

    float scaleFactor;
};
