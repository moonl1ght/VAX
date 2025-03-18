//
// Created by Alexander Lakhonin on 17.02.2025.
//

#ifndef Scene_hpp
#define Scene_hpp

#include <stdio.h>
#include "Model.hpp"
#include "Camera.hpp"
#include "MTLStack.hpp"

class Scene {
public:
  Camera camera = Camera();

  explicit Scene(MTLStack* mtlStack);
  virtual ~Scene();

  Scene(const Scene& rhs) = delete;
  Scene(const Scene&& rhs) = delete;

  Scene& operator=(Scene& rhs) = delete;
  Scene& operator=(Scene&& rhs) = delete;

  void loadScene(std::string bundleResourcesPath);
  void addModel(std::unique_ptr<Model>);
  const std::vector<std::unique_ptr<Model>>& models() const noexcept;
  const std::vector<Light>& lights() const noexcept;

private:
  MTLStack* _mtlStack;

  std::vector<std::unique_ptr<Model>> _models;
  std::vector<Light> _lights;
};

#endif /* Scene_hpp */
