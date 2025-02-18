//
// Created by Alexander Lakhonin on 17.02.2025.
//

#include "Scene.hpp"
#include <iostream>

using namespace std;

Scene::Scene() { }

Scene::~Scene() {
  cout << "delete scene" << endl;
}

void Scene::addModel(unique_ptr<Model> model) {
  _models.push_back(std::move(model));
}

const vector<unique_ptr<Model>>& Scene::models() const noexcept {
  return _models;
}
