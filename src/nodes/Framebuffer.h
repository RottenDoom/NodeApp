#pragma once

#include "Texture.h"
#include "includes.h"

class Framebuffer {
public:
    GLuint fbo = 0;
    Texture colorAttachment;
    GLuint rbo = 0;

    // Framebuffer(int width, int height);
    void create(int width, int height);
    void bind();
    void unbind();
    void destroy();

    Texture getTexture() const {
        return colorAttachment;
    }
};