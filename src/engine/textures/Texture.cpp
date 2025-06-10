#include "Texture.hpp"

void Texture::loadImageView() {
    textureImageView = VKUtils::createImageView(device, textureImage, VK_FORMAT_R8G8B8A8_SRGB);
}