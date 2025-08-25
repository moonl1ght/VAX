//
// Created by Alexander Lakhonin on 21.03.2025.
//

#ifndef Settings_h
#define Settings_h

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "CommonUtils.h"

using json = nlohmann::json;

struct Settings {
  enum class LightRenderingType {
    phong, pbr
  };

  bool debugLight;
  LightRenderingType lightRenderingType;
};

inline Settings loadSettings(std::string filePath) {
  std::ifstream f(filePath);
  json settingsJson = json::parse(f);

  Settings settings;

  settings.debugLight = settingsJson["debugLight"];
//  map_init
  std::map<std::string, Settings::LightRenderingType> lightRenderingTypeMap = {
    {"phong", Settings::LightRenderingType::phong},
    {"PBR", Settings::LightRenderingType::pbr}
  };
  auto lightRendering = lightRenderingTypeMap.find(settingsJson["lightRendering"]);
  if (lightRendering == lightRenderingTypeMap.end()) {
    settings.lightRenderingType = Settings::LightRenderingType::phong;
  } else {
    settings.lightRenderingType = lightRendering->second;
  }
  std::cout << "Loaded settings: " <<
  "debugLight: " << settings.debugLight << "; " <<
  "lightRenderingType: " << as_integer(settings.lightRenderingType) <<
  std::endl;
  return Settings();
}

#endif /* Settings_h */
