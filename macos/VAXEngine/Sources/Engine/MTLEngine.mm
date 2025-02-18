//
// Created by Alexander Lakhonin on 10.02.2025.
//

#include <QuartzCore/QuartzCore.hpp>
#import "MTLEngine.h"
#include "MTLRenderer.hpp"
#include "TESTCPP.hpp"
#include "MTLStack.hpp"
#include "Scene.hpp"
#include "Primitives.hpp"

@implementation MTLEngine {
  MTLRenderer* renderer;
  MTLStack* mtlStack;
  CA::MetalLayer* metalLayer;
  Scene* scene;
}

-(id) init {
  if (self = [super init]) {
    mtlStack = new MTLStack();
    scene = new Scene();
    renderer = new MTLRenderer(mtlStack, scene);
  }
//  runTESTCPP();
  return self;
}

-(void) dealloc {
  NSLog(@"MTLEngine dealloc");
  delete renderer;
  renderer = nullptr;
  delete mtlStack;
  mtlStack = nullptr;
  delete scene;
  scene = nullptr;
}

- (void)configure: (MTKView *)view {
  view.device = (__bridge id<MTLDevice>)(&mtlStack->device());
  view.preferredFramesPerSecond = 60;
  view.clearColor = MTLClearColor(0, 0, 0, 0);
  view.depthStencilPixelFormat = MTLPixelFormatDepth32Float;
  view.sampleCount = 1;
}

- (CAMetalLayer *)getRenderingLayer {
  return (__bridge CAMetalLayer *)metalLayer;
}

- (void)load {
  scene->addModel(std::make_unique<Model>(Primitives::createRGBTriangle(mtlStack->device())));
}

- (void) drawIn: (MTKView *)view {
  CA::MetalLayer* layer = (__bridge CA::MetalLayer*)view.layer;
  renderer->draw(layer);
}

@end
