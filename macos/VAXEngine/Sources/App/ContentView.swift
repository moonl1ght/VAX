//
// Created by Alexander Lakhonin on 03.02.2025.
//

import SwiftUI

struct ContentView: View {
  @State private var isViewerShown = false

  var body: some View {
    if isViewerShown {
      Viewer(isViewerShown: $isViewerShown)
    } else {
      Button(action: { isViewerShown.toggle() } ) {
        Text("Show Viewer")
      }
    }
  }
}
