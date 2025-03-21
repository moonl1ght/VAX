//
// Created by Alexander Lakhonin on 10.02.2025.
//

#include <QuartzCore/QuartzCore.hpp>
#import "MTLEngine.h"
#include "MTLRenderer.hpp"
#include "MTLStack.hpp"
#include "Scene.hpp"
#include "Primitives.hpp"
#include <iostream>
#include <algorithm>
#include "Settings.hpp"

using json = nlohmann::json;

@implementation AppPathes { }
@end

@implementation MTLEngine {
  MTLRenderer* renderer;
  MTLStack* mtlStack;
  CA::MetalLayer* metalLayer;
  Scene* scene;
}

-(id) init {
  if (self = [super init]) {
    mtlStack = new MTLStack();
    scene = new Scene(mtlStack);
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
  [self loadSettings];
//  NSLog(self.appPathes.bundlePath);
//  std::cout << "path: " << self.appPathes.bundlePath << std::endl;
//  auto cube = std::make_unique<Model>(Primitives::createRGBCube(mtlStack->device()));
////  cube->mesh().transform.rotation.setZ(radians_from_degrees(45));
//  scene->addModel(std::move(cube));
  scene->camera.setPosition({3.0f, 3.0f, -3.0f});
  scene->loadScene(std::string([self.appPathes.bundlePath UTF8String]));
//  float z = -10;
//  float x = 5;
//  float y = 5;
////  scene->camera.transform.rotation.setY(std::max(std::atan2(z, x) - (float)M_PI_2, std::atan2(z, x)));
//  scene->camera.transform.rotation.setY(std::atan2(x, z));
//  scene->camera.transform.position.x = x;
//
//  scene->camera.transform.position.y = y;
//  bool isM = z < 0 && y < 0;
//  auto xr = z < 0 ? std::atan2(y, z) + (isM ? (float)M_PI : -(float)M_PI): -std::atan2(y, z);
////  xr = y < 0 ? xr : xr - (float)M_PI;
////  auto xr = std::max((float)-M_PI_2, std::min(-std::atan2(y, z) - (float)M_PI, (float)M_PI_2));
//  scene->camera.transform.rotation.setX(xr);
//  scene->camera.transform.position.z = z;
//  std::cout << "init rotation: " << scene->camera.transform.rotation.x() << " y: " << scene->camera.transform.rotation.y() << " z: " << scene->camera.transform.rotation.z() << std::endl;
}

- (void)resize:(CGSize)viewSize drawableSize:(CGSize)drawableSize {
  renderer->resize(vax::Size{viewSize.width, viewSize.height}, vax::Size{viewSize.width, viewSize.height});
}

- (void)drawIn: (MTKView *)view {
  CA::MetalLayer* layer = (__bridge CA::MetalLayer*)view.layer;
  renderer->draw(layer);
}

- (void)handleDragGesture: (vax::DragGestureValue)value {
//  std::cout << "handleDragGesture: " << scene->camera.transform.position.x << " y: " << scene->camera.transform.position.y << " z: " << scene->camera.transform.position.z << std::endl;
//  std::cout << "rotation: " << scene->camera.transform.rotation.x() << " y: " << scene->camera.transform.rotation.y() << " z: " << scene->camera.transform.rotation.z() << std::endl;
  scene->camera.rotate(value.dragTranslationDelta.vec());
}

- (void)handleAppEvent:(AppEvent)event {
  switch (event) {
    case settingsSaved:
      [self loadSettings];
      break;

    default:
      break;
  }
  std::cout << "event: " << event << std::endl;
}

- (void) loadSettings {
  Settings settings = loadSettings(std::string([self.appPathes.documentPath UTF8String]) + "/settings.json");
}

@end
