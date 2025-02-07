//
// Created by Alexander Lakhonin on 03.02.2025.
//

#ifndef MTLRenderer_hpp
#define MTLRenderer_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

class MTLRenderer
{
public:
  explicit MTLRenderer(MTL::Device* device);
  virtual ~MTLRenderer();

  void draw(CA::MetalLayer* layer);

private:
  MTL::Device* _device;
  MTL::CommandQueue* _commandQueue;
};

#endif /* MTLRenderer_hpp */
