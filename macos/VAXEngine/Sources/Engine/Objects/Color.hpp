//
// Created by Alexander Lakhonin on 07.03.2025.
//

#ifndef Color_hpp
#define Color_hpp

#include <simd/simd.h>

struct Color {
  float r;
  float g;
  float b;

  Color(float r, float g, float b): r(r), g(g), b(b) { };

  static Color black() { return Color(0,0,0); };
  static Color white() { return Color(1,1,1); };
  static Color red() { return Color(1,0,0); };
  static Color green() { return Color(0,1,0); };
  static Color blue() { return Color(0,0,1); };

  inline simd_float3 simd_vec() { return simd_make_float3(r, g, b); };
};

#endif /* Color_hpp */
