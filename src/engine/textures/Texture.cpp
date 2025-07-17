#include "Texture.hpp"

void Texture::loadImageView() {
    textureImageView = VKUtils::createImageView(_device, textureImage, format, aspectMask);
}