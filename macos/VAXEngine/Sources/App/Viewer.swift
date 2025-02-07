//
// Created by Alexander Lakhonin on 03.02.2025.
//

import Foundation
import SwiftUI

struct Viewer: View {
  @Binding var isViewerShown: Bool
  @StateObject private var presenter = Presenter()

  var body: some View {
    
    MetalView(renderer: presenter.renderer)
  }
}

extension Viewer {
  final class Presenter: ObservableObject {
    let renderer = EngineWrapper()
  }
}
