//
// Created by Alexander Lakhonin on 28.03.2025.
//

#ifndef Texture_hpp
#define Texture_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>

namespace vax {
  
  class Texture {
  public:
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
