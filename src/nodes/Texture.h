#pragma once

#include "includes.h"

class Texture {
public:
    GLuint textureid = 0;
    unsigned int width = 0;
    unsigned int height = 0;
    const char* path;

    Texture(GLuint* id, unsigned int* width, unsigned int* height, const char* path);

    cv::Mat originalImage;
    cv::Mat previewImage;

    bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, unsigned int* out_width, unsigned int* out_height);
    bool LoadTextureFromFile(const char* file_name, GLuint* out_texture, unsigned int* out_width, unsigned int* out_height);

private:
    float scale = 0.4f;
};