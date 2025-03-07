//
// Created by Alexander Lakhonin on 03.02.2025.
//

import Foundation
import SwiftUI

struct Viewer: View {
  @Binding var isViewerShown: Bool
  @StateObject private var presenter = Presenter()

  var body: some View {
    MetalView(renderer: presenter.engine)
      .gesture(GestureBuilder.buildDragGestureWithInputController(presenter.engine.inputController))
  }
}

extension Viewer {
  final class Presenter: ObservableObject {
    let engine = EngineWrapper()

    init() {
      engine.load()
    }
  }
}
