//
// Created by Alexander Lakhonin on 03.02.2025.
//

import Foundation
import MetalKit

final class EngineWrapper: MTKViewRenderer {
  let inputController: InputController
  private let engine = MTLEngine()

  init() {
    let appPathes = AppPathes()
    appPathes.bundlePath = Bundle.main.resourcePath ?? ""
    engine.appPathes = appPathes
    inputController = InputController(engine: engine)
  }

  func load() {
    engine.load()
  }

  func configure(_ mtkView: MTKView) {
//    mtkView.
    engine.configure(mtkView)
  }
  
  func resizeView(drawableSize: CGSize, viewportSize: CGSize) {
    let scaleFactor = NSScreen.main?.backingScaleFactor ?? 1
    engine.resize(
      CGSize(width: viewportSize.width * scaleFactor, height: viewportSize.height * scaleFactor),
      drawableSize:  CGSize(width: drawableSize.width * scaleFactor, height: drawableSize.height * scaleFactor)
    )
  }
  
  func draw(in mtkView: MTKView) {
    engine.draw(in: mtkView)
  }
}
