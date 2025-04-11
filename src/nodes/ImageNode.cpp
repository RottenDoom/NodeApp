#include "ImageNode.h"
#include "Texture.h"

ImageNode::ImageNode(const char* imagePath)
    : texture(&id, &w, &h, imagePath)
    // framebuffer(texture.width, texture.height)
{
    loaded = texture.textureid != 0;
    if (!loaded) {
        std::cerr << "Failed to load image\n";
        return;
    }

    this->nodeId = 0;

    // framebuffer.create(texture.width, texture.height);
    // renderToFramebuffer();
}

ImageNode::~ImageNode()
{
    // framebuffer.destroy();
}

void ImageNode::renderToFramebuffer() {
    
}

void ImageNode::renderUI() {
    if (!loaded) {
        ImGui::Text("Image not loaded.");
        return;
    }

    ImVec2 viewportMin = ImGui::GetWindowPos(); // Top-left of the viewport
    ImVec2 viewportMax = { viewportMin.x + ImGui::GetWindowSize().x, viewportMin.y + ImGui::GetWindowSize().y }; // Bottom-right

    m_ViewportBounds[0] = viewportMin;
    m_ViewportBounds[1] = viewportMax;

    ImGui::Begin("Image Node");
    ImGui::Text("Image from FBO:");
    ImGui::Image((ImTextureID)(intptr_t)id, ImVec2((float)texture.width, (float)texture.height));
    ImGui::End();
}
    // ~ImageNode();

    // void renderUI(); // renders the node using ImGui
    // void render();   // if you later want to render it manually