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

  ModelLoader modelLoader = ModelLoader(_mtlStack);
  modelLoader.bundleResourcesPath = bundleResourcesPath;
  auto model = std::make_unique<Model>(modelLoader.loadModel("/helmet.obj"));
  addModel(std::move(model));

  _lights = {LightBulder::sunlight()};
  _gizmo = Primitives::createGizmo(_mtlStack->device());
}

void Scene::addModel(unique_ptr<Model> model) {
  _models.push_back(std::move(model));
}

const vector<unique_ptr<Model>>& Scene::models() const noexcept {
  return _models;
}

const vector<Light>& Scene::lights() const noexcept {
  return _lights;
}


const BaseModel& Scene::gizmo() const noexcept {
  return *_gizmo;
}
