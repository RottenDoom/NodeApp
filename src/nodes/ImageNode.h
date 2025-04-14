#pragma once
#include "Texture.h"
#include "Node.h"
#include "NodeManager.h"
#include "Framebuffer.h"

class ImageNode : public Node
{
public:
    ImageNode(const int nodeid, const char* imagePath);
    ~ImageNode();

    void OnRender(); // renders the node using ImGui
    void OnUpdate();
    void RenderProperties();
    // void render();

    const int GetId() const;
    ImVec2 GetInputSocketPos();
    ImVec2 GetOutputSocketPos();
    void renderToFramebuffer();
    cv::Mat GetOutputImage(int fromNodeId);

    int nodeId;
    unsigned int w = 0;
    unsigned int h = 0;
    GLuint id = 0;
    Texture texture;

private:
    ImVec2 m_ViewportBounds[2];
    // Framebuffer framebuffer;
    bool loaded = false;

    void InitializeSockets();
    void SetNodeSockets(ImVec2 nodeSize, ImVec2 nodePos);
};