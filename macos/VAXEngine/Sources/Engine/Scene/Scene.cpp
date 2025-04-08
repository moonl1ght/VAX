//
// Created by Alexander Lakhonin on 17.02.2025.
//

#include "Scene.hpp"
#include <iostream>

#include "LightBuilder.hpp"
#include "ModelLoader.hpp"
#include "Primitives.hpp"

using namespace std;

Scene::Scene(MTLStack* mtlStack): _mtlStack(mtlStack) { }

Scene::~Scene() {
  cout << "delete scene" << endl;
}

void Scene::loadScene(std::string bundleResourcesPath) {

  camera.setPosition({1.0f, 1.0f, -3.0f});
  ModelLoader modelLoader = ModelLoader(_mtlStack);
  modelLoader.bundleResourcesPath = bundleResourcesPath;
  auto model = std::make_unique<Model>(modelLoader.loadModel("/helmet.obj"));
  auto lplane = std::make_unique<Model>(modelLoader.loadModel("/large_plane.obj"));
  auto plane = Primitives::plane(_mtlStack->device());
  plane->transform.position = {0.0f, -1.0f, 0.0f};
  lplane->transform.position = {0.0, -0.3f, 0.0f};
  addModel(std::move(model));
  addModel(std::move(lplane));
//  addModel(std::move(plane));

//  camera.projection = Camera::Projection::orthographic;
  camera.farPlane = 10;
  camera.viewSize = 10;
  camera.setPosition({-2.0f, 1.0f, -2.0f});
//  camera.projection = Camera::Projection::orthographic;
  shadowCamera.farPlane = 10;

  _lights = {LightBulder::sunlight()};
  shadowCamera.update(camera, _lights[0].position);
//  shadowCamera.setPosition({-1.0f, 1.0f, 3.0f});
  _gizmo = Primitives::createGizmo(_mtlStack->device());
}

void Scene::addModel(unique_ptr<BaseModel> model) {
  _models.push_back(std::move(model));
}

const vector<unique_ptr<BaseModel>>& Scene::models() const noexcept {
  return _models;
}

const vector<Light>& Scene::lights() const noexcept {
  return _lights;
}


const BaseModel& Scene::gizmo() const noexcept {
  return *_gizmo;
}
