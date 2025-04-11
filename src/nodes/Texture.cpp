#include "Texture.h"

Texture::Texture(GLuint *id, unsigned int *width, unsigned int *height, const char *path)
{
    LoadTextureFromFile(path, id, width, height);
}

bool Texture::LoadTextureFromMemory(const void *data, size_t data_size, GLuint *out_texture, unsigned int *out_width, unsigned int *out_height)
{
    std::vector<uchar> buffer((uchar*)data, (uchar*)data + data_size);
    cv::Mat image = cv::imdecode(buffer, cv::IMREAD_UNCHANGED); // Decode from memory

    if (image.empty()) {
        std::cerr << "cv::imdecode failed. Possibly invalid image data.\n";
    }
    // } else { //debuging
    
    //     std::cout << "Image loaded: " << image.cols << "x" << image.rows << " channels: " << image.channels() << "\n";
    // }


    unsigned int image_width = image.cols;
    unsigned int image_height = image.rows;
    void* image_data = image.data;

    GLenum format = image.channels() == 4 ? GL_RGBA : GL_RGB;
    GLenum inputFormat = image.channels() == 4 ? GL_BGRA : GL_BGR;
    
    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, image_width, image_height, 0, inputFormat, GL_UNSIGNED_BYTE, image_data);

    // update the data
    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    textureid = image_texture;
    width = image_width;
    height = image_height;

    return true;
}

bool Texture::LoadTextureFromFile(const char *file_name, GLuint *out_texture, unsigned int *out_width, unsigned int *out_height)
{
    FILE* f = fopen(file_name, "rb");
    if (f == NULL)
        return false;
    fseek(f, 0, SEEK_END);
    size_t file_size = (size_t)ftell(f);
    if (file_size == -1)
        return false;
    fseek(f, 0, SEEK_SET);
    void* file_data = IM_ALLOC(file_size);
    fread(file_data, 1, file_size, f);
    fclose(f);
    bool ret = LoadTextureFromMemory(file_data, file_size, out_texture, out_width, out_height);
    IM_FREE(file_data);
    return ret;
}
