#include "Texture.h"

Texture::Texture(std::string texturePath, unsigned int format, bool flip)
{
    width = 0;
    height = 0;
    nrChannels = 0;
    //pass in the address of textureID
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    //different parameters for how to handle edges of a texture for each axis and texture filter options for close and far objects - pixel sampling rules, mip map usage
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);    
    //float borderColor[] = { 0.1f, 0.3f, 1.0f, 1.0f};
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //create texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {

        std::cout << "Success opening picture. Height: " << height << " Width: " << width << std::endl;
        //generate texture

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        //automatically generate mip maps which are used as sampling objects when objects are further away to save some memory and performance
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Error while loading image" << std::endl;
    }
    //free up the data pointer  
    stbi_image_free(data);
}

void Texture::use(GLuint textureSlot)
{
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}
