//
// Created by Alexander Lakhonin on 05.03.2025.
//

import Foundation
import SwiftUI

protocol DragGestureInputController: AnyObject {
  func updateDragGesture(_ value: DragGesture.Value)
  func endDragGesture(_ value: DragGesture.Value)
}

final class InputController {
  private let engine: MTLEngine

  private var dragTouchLocation: CGPoint?
  private var dragTranslation: CGSize = .zero
  private var dragTranslationDelta: CGSize = .zero

  init(engine: MTLEngine) {
    self.engine = engine
  }
}

extension InputController: DragGestureInputController {
  func updateDragGesture(_ value: DragGesture.Value) {
    dragTouchLocation = value.location
    dragTranslationDelta = CGSize(
      width: value.translation.width - dragTranslation.width,
      height: value.translation.height - dragTranslation.height)
    dragTranslation = value.translation
    if abs(value.translation.width) > 1 || abs(value.translation.height) > 1 {
      dragTouchLocation = nil
    }
    engine.handleDragGesture(
      vax.DragGestureValue(
        dragTouchLocation: dragTouchLocation?.vaxPoint ?? vax.Point(-1, -1),
        dragTranslation: dragTranslation.vaxSize,
        dragTranslationDelta: dragTranslationDelta.vaxSize
      )
    )

  }

  func endDragGesture(_ value: DragGesture.Value) {
    dragTranslation = .zero
  }
}

extension CGSize {
  var vaxSize: vax.Size {
    vax.Size(Float(width), Float(height))
  }
}

extension CGPoint {
  var vaxPoint: vax.Point {
    vax.Point(Float(x), Float(y))
  }
}

