//
// Created by Alexander Lakhonin on 07.03.2025.
//

#ifndef LightBuilder_hpp
#define LightBuilder_hpp

#include "ShadersCommon.h"
#include "Color.hpp"

namespace LightBulder {
  Light sunlight() {
    Light light = Light();
    light.position = {-2.0f, 2.0f, -2.0f};
    light.color = Color::white().simd_vec();
    light.specularColor = {0.6f, 0.6f, 0.6f};
    light.attenuation = {1.0f, 0.0f, 0.0f};
    light.type = Sunlight;
    return light;
  }

  Light ambient() {
    Light light = sunlight();
    light.color = {0.05f, 0.1f, 0.0f};
    light.type = Ambientlight;
    return light;
  }

  Light pointlight() {
    Light light = sunlight();
    light.color = {1.0f, 0.0f, 0.0f};
    light.type = Pointlight;
    light.attenuation = {0.5f, 2.0f, 1.0f};
    light.position = {-0.8, 0.76, -1.1};
    return light;
  }

  Light spotlight() {
    Light light = sunlight();
    light.color = {1.0f, 1.0f, 0.0f};
    light.position = {-0.8, 0.76, -1.1};
    light.attenuation = {1.0f, 0.5f, 0.0f};
    light.coneAngle = radians_from_degrees(40);
    light.coneAttenuation = 8;
    light.type = Spotlight;
    return light;
  }
}

#endif /* LightBuilder_hpp */
