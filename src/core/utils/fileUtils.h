#pragma once

#include <filesystem>
#include <optional>

namespace vax::core::utils {
std::string getCurrentDatetimeString();

std::optional<std::filesystem::path> getLatestFolder(const std::filesystem::path& parent_dir);
} // namespace vax::core::utils