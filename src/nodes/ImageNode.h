#pragma once
#include "Texture.h"
#include "Framebuffer.h"
#include <string>

class ImageNode {
public:
    ImageNode(const char* imagePath);
    ~ImageNode();

    void renderUI(); // renders the node using ImGui
    // void render();
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
};