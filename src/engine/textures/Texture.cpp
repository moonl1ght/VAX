#include "Texture.hpp"
#include "TextureLoader.hpp"

void Texture::loadImageView() {
    textureImageView = TextureLoader::createImageView(_device, textureImage, format, aspectMask).value();
}