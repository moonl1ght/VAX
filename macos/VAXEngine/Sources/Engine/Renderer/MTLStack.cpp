//
// Created by Alexander Lakhonin on 12.02.2025.
//

#include "MTLStack.hpp"
#include <Foundation/Foundation.hpp>
#include <sys/sysctl.h>

using namespace MTL;

MTLStack::MTLStack()
: _device(CreateSystemDefaultDevice())
, _commandQueue(_device->newCommandQueue())
, _library(_device->newDefaultLibrary())
{ }

MTLStack::~MTLStack() {
  _device->release();
  _device = nullptr;
  _commandQueue->release();
  _commandQueue = nullptr;
  _library->release();
  _library = nullptr;
}

Device& MTLStack::device() const {
  return *_device;
}

CommandQueue& MTLStack::commandQueue() const {
  return *_commandQueue;
}

Library& MTLStack::library() const {
  return *_library;
}
