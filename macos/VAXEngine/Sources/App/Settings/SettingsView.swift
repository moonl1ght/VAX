//
// Created by Alexander Lakhonin on 20.03.2025.
//

import SwiftUI

struct SettingsView: View {
  @StateObject private var presenter = Presenter()

  var body: some View {
    VStack {
      settingsPicker(name: "Light", selection: $presenter.settingsModel.lightRendering, SettingsModel.LightRendering.allCases, id: \.rawValue)
      Toggle("Debug Light", isOn: $presenter.settingsModel.debugLight)
      Spacer()
      HStack {
        Button(action: { presenter.save() } ) {
          Text("Save")
        }
        Button(action: { presenter.reset() } ) {
          Text("Reset to default")
        }
      }
    }
    .frame(width: 300)
    .padding(30)
  }

  @ViewBuilder
  private func settingsPicker<SelectionValue: Hashable, Data: RandomAccessCollection, ID: Hashable>(
    name: String, selection: Binding<SelectionValue>, _ data: Data, id: KeyPath<Data.Element, ID>
  ) -> some View where Data.Element: Hashable & RawValueable {
    Picker(name, selection: selection) {
      ForEach(data, id: id) { item in
        Text(item.rawValue).tag(item)
      }
    }
  }
}

private extension SettingsView {
  final class Presenter: ObservableObject {
    @Published var settingsModel = SettingsModel()

    private let decoder = JSONDecoder()
    private let encoder = JSONEncoder()

    init() {
      guard let dir = FileManager.documentsDir, let url = URL(string: dir + "/settings.json") else {
        debugPrint("🚨 Failed to load settings")
        return
      }
      do {
        let jsonString = try String(contentsOfFile: dir + "/settings.json", encoding: .utf8)
        guard let data = jsonString.data(using: .utf8) else {
          debugPrint("🚨 Failed to load settings")
          return
        }
        settingsModel = try decoder.decode(SettingsModel.self, from: data)
      } catch {
        debugPrint("🚨 Failed to load settings \(error)")
      }
    }

    func save() {
      guard let dir = FileManager.documentsDir else {
        debugPrint("🚨 Failed to save settings")
        return
      }
      do {
        let data = try encoder.encode(settingsModel)
        debugPrint("Saving settings \(settingsModel) to \(dir)")
        guard let json = String(data: data, encoding: .utf8) else {
          debugPrint("🚨 Failed to save settings, failed to create json string")
          return
        }
        try json.write(toFile: dir + "/settings.json", atomically: true, encoding: .utf8)
        debugPrint("Saved settings \(settingsModel) to \(dir)")
        NotificationCenter.default.post(name: NSNotification.Name(rawValue: "VAXEngine.settings.saved"), object: nil)
      } catch {
        debugPrint("🚨 Failed to save settings \(error)")
      }
    }

    func reset() {
      settingsModel = SettingsModel()
      save()
    }
  }
}
