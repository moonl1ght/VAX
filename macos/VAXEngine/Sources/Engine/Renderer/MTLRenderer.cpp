//
// Created by Alexander Lakhonin on 03.02.2025.
//

#include "MTLRenderer.hpp"
#include <iostream>

using namespace std;
using namespace MTL;

MTLRenderer::MTLRenderer(Device* device)
: _device(device)
, _commandQueue(device->newCommandQueue())
{
  cout << "init renderer" << endl;
};

MTLRenderer::~MTLRenderer() {
  _commandQueue->release();
  _commandQueue = nullptr;
  cout << "deinit renderer" << endl;
};

void MTLRenderer::draw(CA::MetalLayer *layer) {
  cout << "draw" << endl;
  CommandBuffer* commandBuffer = _commandQueue->commandBuffer();

//  const Drawable* drawble;
  commandBuffer->presentDrawable(layer->nextDrawable());
  commandBuffer->commit();
};

//void MTLRenderer::commitDraw(const Drawable* drawable) {
//
//};

