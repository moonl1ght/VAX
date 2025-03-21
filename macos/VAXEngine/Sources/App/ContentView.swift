//
// Created by Alexander Lakhonin on 03.02.2025.
//

import SwiftUI

struct ContentView: View {
  @Environment(\.openWindow) var openWindow
  @State private var isViewerShown = false

  var body: some View {
    if isViewerShown {
      Viewer(isViewerShown: $isViewerShown)
    } else {
      VStack {
        Button(action: { isViewerShown.toggle() } ) {
          Text("Show Viewer")
        }
        Button(action: { openWindow(id: "settings-window") } ) {
          Text("Show Settings")
        }
      }
    }
  }
}
