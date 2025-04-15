#pragma once

#include "Node.h"

// Splits the picture into its isolated color models
class ColorSplitterNode : public Node {
public:
    ColorSplitterNode(int id);
    ~ColorSplitterNode();

    enum class OutputMode {
        RGB,        // RGB mode: isolated color channels with only one channel nonzero per output.
        Grayscale,  // Grayscale mode: each output shows the channel as a grayscale image.
        HSV         // HSV mode: input is converted to HSV; outputs represent Hue, Saturation, and Value.
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
    cv::Mat outputChannels[3];      // Stores output for each channel
    OutputMode mode;
    int selectedChannel;

    void ProcessRGBMode(const cv::Mat& input);
    void ProcessGrayscaleMode(const cv::Mat& input);
    void ProcessHSVMode(const cv::Mat& input);

    void UpdateTexture();
    void SetNodeSockets(ImVec2 nodeSize, ImVec2 nodePos);
    void InitializeSockets();

    cv::Mat inputImage;
    cv::Mat outputImage;  // potentially overlayed

    float scaleFactor;
};
