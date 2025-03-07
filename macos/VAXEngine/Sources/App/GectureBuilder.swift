//
// Created by Alexander Lakhonin on 05.03.2025.
//

import Foundation
import SwiftUI

enum GestureBuilder {
  static func buildDragGestureWithInputController(_ inputController: DragGestureInputController) -> some Gesture {
    DragGesture(minimumDistance: 0.1)
      .onChanged(inputController.updateDragGesture(_:))
      .onEnded(inputController.endDragGesture(_:))
  }
}
