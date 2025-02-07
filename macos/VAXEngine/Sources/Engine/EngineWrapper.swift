//
// Created by Alexander Lakhonin on 03.02.2025.
//

import Foundation
import MetalKit

final class EngineWrapper: MTKViewRenderer {
  private let engine = MTLEngine()

  func load() {
    engine.load()
  }

  func configure(_ mtkView: MTKView) {
    engine.configure(mtkView)
  }
  
  func resizeView(drawableSize: CGSize, viewportSize: CGSize) {
  }
  
  func draw(in mtkView: MTKView) {
    engine.draw(in: mtkView)
  }
}
