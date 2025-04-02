//
// Created by Alexander Lakhonin on 17.02.2025.
//

#ifndef Scene_hpp
#define Scene_hpp

#include <stdio.h>
#include "Model.hpp"
#include "Camera.hpp"
#include "ShadowCamera.hpp"
#include "MTLStack.hpp"
#include "BaseModel.hpp"

class Scene {
public:
  Camera camera = Camera();
  ShadowCamera shadowCamera = ShadowCamera();


  explicit Scene(MTLStack* mtlStack);
  virtual ~Scene();

  Scene(const Scene& rhs) = delete;
  Scene(const Scene&& rhs) = delete;

  Scene& operator=(Scene& rhs) = delete;
  Scene& operator=(Scene&& rhs) = delete;

  void loadScene(std::string bundleResourcesPath);
  void addModel(std::unique_ptr<BaseModel>);
  const std::vector<std::unique_ptr<BaseModel>>& models() const noexcept;
  const std::vector<Light>& lights() const noexcept;
  const BaseModel& gizmo() const noexcept;

private:
  MTLStack* _mtlStack;

  std::vector<std::unique_ptr<BaseModel>> _models;
  std::vector<Light> _lights;
  std::unique_ptr<BaseModel> _gizmo;
};

#endif /* Scene_hpp */
