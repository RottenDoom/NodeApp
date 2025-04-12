#pragma once
#include "Texture.h"
#include "Node.h"
#include "NodeManager.h"
#include "Framebuffer.h"
#include <string>

class ImageNode : public Node
{
public:
    ImageNode(const char* imagePath);
    ~ImageNode();

    void OnRender(); // renders the node using ImGui
    void OnUpdate();
    // void render();

    ImVec2 GetInputSocketPos();
    ImVec2 GetOutputSocketPos();
    void renderToFramebuffer();

    int nodeId;
    unsigned int w = 0;
    unsigned int h = 0;
    GLuint id = 0;

private:
    ImVec2 m_ViewportBounds[2];
    Texture texture;
    // Framebuffer framebuffer;
    bool loaded = false;

    void InitializeSockets();
    void SetNodeSockets(ImVec2 nodeSize, ImVec2 nodePos);
};