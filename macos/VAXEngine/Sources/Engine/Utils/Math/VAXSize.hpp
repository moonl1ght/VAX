//
// Created by Alexander Lakhonin on 04.03.2025.
//

#ifndef VAXSize_hpp
#define VAXSize_hpp

#include <stdio.h>

struct VAXSize {
  double width;
  double height;

  inline double whRatio() const { return width / height; };
};

#endif /* VAXSize_hpp */
