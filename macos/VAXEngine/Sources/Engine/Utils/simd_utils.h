//
// Created by Alexander Lakhonin on 13.03.2025.
//

#ifndef simd_utils_h
#define simd_utils_h

namespace std {
  template<> struct hash<simd::float2> {
    size_t operator()(simd::float2 const& vector) const {
      size_t h1 = hash<float>{}(vector.x);
      size_t h2 = hash<float>{}(vector.y);
      return h1 ^ (h2 << 1);
    }
  };

  template<> struct hash<simd::float3> {
    size_t operator()(simd::float3 const& vector) const {
      size_t h1 = hash<float>{}(vector.x);
      size_t h2 = hash<float>{}(vector.y);
      size_t h3 = hash<float>{}(vector.z);
      return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
  };
}

#endif /* simd_utils_h */
