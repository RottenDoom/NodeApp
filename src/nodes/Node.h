#pragma once
#include "includes.h"

class Node {
public:
    Node(const int nodeid) : nodeId(nodeid) {}
    virtual ~Node() = default;

    enum Type {
        IMAGE,
        BLUR,
        CONTRAST_AND_BRIGHTNESS,
        COLOR_SPLITTER,
        THRESHOLD,
        EDGE_DETECTION,
        BLEND_NODE,
        NOISE_GEN,
        CONVULATIONAL,
    };

    virtual void OnRender() = 0;
    virtual void OnUpdate() = 0;
    virtual void RenderProperties() = 0;
    Type type;
    struct NodeSocket {
        ImVec2 position;
        enum class SocketType { Input, Output };
        int index; // for nodes with multiple inputs
        SocketType type;
    };

    const int GetId() const {
        return this->nodeId;
    }
    
    virtual ImVec2 GetInputSocketPos() = 0;
    virtual ImVec2 GetOutputSocketPos() = 0;
    virtual cv::Mat GetOutputImage(int fromId) = 0;
    std::vector<NodeSocket> GetInputSockets() {
        return inputSockets;
    }
    Type GetType() { return type; }
    

    int nodeId;
    unsigned int w = 0;
    unsigned int h = 0;
    GLuint id = 0;
    std::uintmax_t fileSize;

protected:
    ImVec2 position;
    std::vector<NodeSocket> inputSockets;
    std::vector<NodeSocket> outputSockets;

    virtual void InitializeSockets() = 0;
};