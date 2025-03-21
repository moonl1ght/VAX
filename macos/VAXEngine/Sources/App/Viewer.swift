//
// Created by Alexander Lakhonin on 03.02.2025.
//

import Foundation
import SwiftUI

struct Viewer: View {
  @Environment(\.openWindow) var openWindow
  @Binding var isViewerShown: Bool
  @StateObject private var presenter = Presenter()

  var body: some View {
    ZStack {
      MetalView(renderer: presenter.engine)
        .gesture(GestureBuilder.buildDragGestureWithInputController(presenter.engine.inputController))
      HStack {
        VStack {
          VStack {
            Button(action: { isViewerShown.toggle() } ) {
              imageWithSystemName("xmark")
            }
            Button(action: { openWindow(id: "settings-window") } ) {
              imageWithSystemName("gear")
            }
          }
          Spacer()
        }
        Spacer()
      }
      .padding(15)
    }
  }

  @ViewBuilder
  private func imageWithSystemName(_ name: String) -> some View {
    Image(systemName: name)
      .frame(width: 24)
  }
}

extension Viewer {
  final class Presenter: ObservableObject {
    let engine = EngineWrapper()

    init() {
      engine.load()

      NotificationCenter.default.addObserver(
        self, selector: #selector(onSettingsSaved), name: NSNotification.Name(rawValue: "VAXEngine.settings.saved"), object: nil
      )
    }

    @objc
    private func onSettingsSaved() {
      engine.sendEvent(settingsSaved)
    }
  }
}
