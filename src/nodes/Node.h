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
    };

    virtual void OnRender() = 0;
    virtual void OnUpdate() = 0;
    virtual void RenderProperties() = 0;
    Type type;

    
    virtual ImVec2 GetInputSocketPos() = 0;
    virtual ImVec2 GetOutputSocketPos() = 0;
    Type GetType() { return type; }
    
    struct NodeSocket {
        int nodeId;
        ImVec2 position;
    };

    const int nodeId;
    unsigned int w = 0;
    unsigned int h = 0;
    GLuint id = 0;
    std::uintmax_t fileSize;

protected:
    ImVec2 position;
    NodeSocket inputSocket;
    NodeSocket outputSocket;
};