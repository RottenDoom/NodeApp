#include "Framebuffer.h"

void Framebuffer::create(int width, int height) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment.id, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    std::cerr << "FBO status: 0x" << std::hex << status << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::destroy() {
    if (rbo) glDeleteRenderbuffers(1, &rbo);
    if (fbo) glDeleteFramebuffers(1, &fbo);
    // colorAttachment.destroy();
    rbo = 0;
    fbo = 0;
}

// Framebuffer::Framebuffer(int width, int height) {
//     create(width, height);
// }