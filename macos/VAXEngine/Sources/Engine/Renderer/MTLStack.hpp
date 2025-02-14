//
// Created by Alexander Lakhonin on 12.02.2025.
//

#ifndef MTLStack_hpp
#define MTLStack_hpp

#include <Metal/Metal.hpp>

class MTLStack {
public:
  explicit MTLStack();
  virtual ~MTLStack();

  MTL::Device& device() const;
  MTL::CommandQueue& commandQueue() const;
  MTL::Library& library() const;

private:
  MTL::Device* _device;
  MTL::CommandQueue* _commandQueue;
  MTL::Library* _library;
};

#endif /* MTLStack_hpp */
