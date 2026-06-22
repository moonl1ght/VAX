#include "fileUtils.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace vax::core::utils {

std::string getCurrentDatetimeString() {
    auto now = std::chrono::system_clock::now();
    std::time_t raw_time = std::chrono::system_clock::to_time_t(now);
    std::tm* time_info = std::localtime(&raw_time);
    std::stringstream ss;
    ss << std::put_time(time_info, "%Y-%m-%d_%H-%M-%S");
    return ss.str();
}

std::optional<std::filesystem::path> getLatestFolder(const std::filesystem::path& parent_dir) {
    namespace fs = std::filesystem;
    fs::path latest_folder;
    fs::file_time_type latest_time;
    bool found = false;

    if (!fs::exists(parent_dir) || !fs::is_directory(parent_dir)) {
        return "";
    }

    for (const auto& entry : fs::directory_iterator(parent_dir)) {
        if (entry.is_directory()) {
            auto current_time = fs::last_write_time(entry);

            if (!found || current_time > latest_time) {
                latest_time = current_time;
                latest_folder = entry.path();
                found = true;
            }
        }
    }

    return latest_folder;
}

} // namespace vax::core::utils
