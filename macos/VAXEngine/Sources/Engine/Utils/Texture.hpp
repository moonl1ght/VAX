//
// Created by Alexander Lakhonin on 28.03.2025.
//

#ifndef Texture_hpp
#define Texture_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>
#include "VAXMathObjects.hpp"

namespace vax {
  
  class Texture {
  public:
    Texture(
      vax::Size size,
      MTL::PixelFormat pixelFormat,
      MTL::Device& device,
      MTL::TextureType textureType = MTL::TextureType2D,
      MTL::TextureUsage usage = MTL::TextureUsageRenderTarget & MTL::TextureUsageShaderRead
    ) {
      MTL::TextureDescriptor* descriptor = MTL::TextureDescriptor::alloc()->init();
      descriptor->setTextureType(textureType);
      descriptor->setPixelFormat(pixelFormat);
      descriptor->setWidth(size.width);
      descriptor->setHeight(size.height);
      descriptor->setUsage(usage);

      _texture = device.newTexture(descriptor);

      descriptor->release();
    };

    Texture(MTL::TextureDescriptor* descriptor, MTL::Device& device)
    : _texture(device.newTexture(descriptor)) { };
    
    ~Texture();
    
    Texture(const Texture& rhs);
    Texture(Texture&& rhs);
    
    Texture& operator=(Texture& rhs);
    Texture& operator=(Texture&& rhs);
    
    MTL::Texture& texture() const noexcept;
    
  private:
    MTL::Texture *_texture;
  };
  
}

#endif /* Texture_hpp */
