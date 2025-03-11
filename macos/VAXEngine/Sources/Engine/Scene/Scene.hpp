//
// Created by Alexander Lakhonin on 17.02.2025.
//

#ifndef Scene_hpp
#define Scene_hpp

#include <stdio.h>
#include "Model.hpp"
#include "Camera.hpp"

class Scene {
public:
  Camera camera = Camera();

  explicit Scene();
  virtual ~Scene();

  Scene(const Scene& rhs) = delete;
  Scene(const Scene&& rhs) = delete;

  Scene& operator=(Scene& rhs) = delete;
  Scene& operator=(Scene&& rhs) = delete;

  void loadScene();
  void addModel(std::unique_ptr<Model>);
  const std::vector<std::unique_ptr<Model>>& models() const noexcept;

private:
  std::vector<std::unique_ptr<Model>> _models;
};

#endif /* Scene_hpp */
