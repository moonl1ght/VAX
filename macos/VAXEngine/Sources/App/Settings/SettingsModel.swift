//
// Created by Alexander Lakhonin on 20.03.2025.
//

import Foundation

protocol RawValueable {
  var rawValue: String { get }
}

struct SettingsModel: Codable {
  enum LightRendering: String, Codable, CaseIterable, RawValueable {
    case phong, PBR
  }

  var lightRendering: LightRendering = .phong
  var debugLight = false
}
