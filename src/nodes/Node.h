#pragma once
#include "includes.h"

class Node {
public:
    Node() = default;
    virtual ~Node() = default;

    virtual void OnRender() = 0;
    virtual void OnUpdate() = 0;

    
    virtual ImVec2 GetInputSocketPos(int index) = 0;
    virtual ImVec2 GetOutputSocketPos(int index) = 0;
    
    struct NodeSocket {
        ImVec2 position;
        int id;
    };

    int nodeId;
    unsigned int w = 0;
    unsigned int h = 0;
    GLuint id = 0;
    bool isDraggingWire;

protected:
    ImVec2 position;
    NodeSocket inputSocket;
    NodeSocket outputSocket;
};