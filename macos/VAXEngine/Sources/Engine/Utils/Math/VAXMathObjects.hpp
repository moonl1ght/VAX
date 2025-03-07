//
// Created by Alexander Lakhonin on 04.03.2025.
//

#ifndef VAXMathObjects_hpp
#define VAXMathObjects_hpp

#include <stdio.h>
#include <simd/simd.h>
#include <concepts>

template<typename T>
concept arithmetic = std::integral<T> or std::floating_point<T>;

namespace vax {
  typedef struct Size {
    float width;
    float height;

    template<typename T>
    requires arithmetic<T>
    Size(T width, T height): width(float(width)), height(float(height)) { };

    inline double whRatio() const { return width / height; };
    inline simd_float2 vec() const { return simd_make_float2(width, height); };
  } Size;

  typedef struct Point {
    float x;
    float y;

    template<typename T>
    requires arithmetic<T>
    Point(T x, T y): x(float(x)), y(float(y)) { };

    inline simd_float2 vec() const { return simd_make_float2(x, y); };
  } Point;
}

#endif /* VAXMathObjects_hpp */
