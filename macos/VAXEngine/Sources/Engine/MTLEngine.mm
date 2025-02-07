//
// Created by Alexander Lakhonin on 10.02.2025.
//

#include <QuartzCore/QuartzCore.hpp>
#import "MTLEngine.h"
#include "MTLRenderer.hpp"


@implementation MTLEngine {
  MTLRenderer* renderer;
  MTL::Device* device;
  CA::MetalLayer* metalLayer;
}

-(id) init {
  if (self = [super init]) {
    device = MTL::CreateSystemDefaultDevice();
    renderer = new MTLRenderer(device);
  }
  return self;
}

-(void) dealloc {
  delete renderer;
  renderer = nullptr;
  device->release();
  device = nullptr;
}

- (void)configure: (MTKView *)view {
  view.device = (__bridge id<MTLDevice>)device;
}

- (CAMetalLayer *)getRenderingLayer {
  return (__bridge CAMetalLayer *)metalLayer;
}

- (void)load { }

- (void) drawIn: (MTKView *)view {
  CA::MetalLayer* layer = (__bridge CA::MetalLayer*)view.layer;
  renderer->draw(layer);
}

@end
