//
// Created by Alexander Lakhonin on 03.02.2025.
//

import Foundation
import MetalKit
import SwiftUI

public protocol MTKViewRenderer: AnyObject {
  func configure(_ mtkView: MTKView)
  func resizeView(drawableSize: CGSize, viewportSize: CGSize)
  func draw(in mtkView: MTKView)
}

public struct MetalView: NSViewControllerRepresentable {
  private let renderer: MTKViewRenderer

  public init(renderer: MTKViewRenderer) {
    self.renderer = renderer
  }

  public func makeNSViewController(context: Context) -> MetalViewController {
    MetalViewController(renderer: renderer)
  }

  public func updateNSViewController(_ nsViewController: MetalViewController, context: Context) { }
}

public final class MetalViewController: NSViewController {
  private let renderer: MTKViewRenderer
  private let mtkView = MTKView(frame: .zero)

  init(renderer: MTKViewRenderer) {
    self.renderer = renderer
    super.init(nibName: nil, bundle: nil)
  }

  @available(*, unavailable)
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }

  public override func viewDidLoad() {
    super.viewDidLoad()
    configureMTKView()
  }

  private func configureMTKView() {
    view.addSubview(mtkView)

    mtkView.delegate = self

    renderer.configure(mtkView)

    mtkView.translatesAutoresizingMaskIntoConstraints = false

    NSLayoutConstraint.activate([
      mtkView.topAnchor.constraint(equalTo: view.topAnchor),
      mtkView.leadingAnchor.constraint(equalTo: view.leadingAnchor),
      mtkView.bottomAnchor.constraint(equalTo: view.bottomAnchor),
      mtkView.trailingAnchor.constraint(equalTo: view.trailingAnchor)
    ])
  }
}

extension MetalViewController: MTKViewDelegate {
  public func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
    renderer.resizeView(drawableSize: size, viewportSize: view.frame.size)
  }

  public func draw(in view: MTKView) {
    renderer.draw(in: view)
  }
}
