//
// Created by Alexander Lakhonin on 20.03.2025.
//

import Foundation

extension FileManager {
  static var documentsDir: String? {
    guard let documents = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first else {
      return nil
    }
    return documents.path
  }
}
