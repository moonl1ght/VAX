//
// Created by Alexander Lakhonin on 05.03.2025.
//

#ifndef DragGestureValue_h
#define DragGestureValue_h

#include <stdio.h>
#include "VAXMathObjects.hpp"

namespace vax {
  struct DragGestureValue {
    vax::Point dragTouchLocation;
    vax::Size dragTranslation;
    vax::Size dragTranslationDelta;
  };
}

#endif /* DragGestureValue_h */
