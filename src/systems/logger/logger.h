#pragma once

#include "fsLogger.h"
#include <iostream>
#include <memory>
#include <sstream>

#define LOG_INFO(message) vax::Logger::getInstance().info(message)
#define LOG_ERROR(message) vax::Logger::getInstance().error(message)
#define LOG_WARNING(message) vax::Logger::getInstance().warning(message)
#define LOG_DEBUG(message) vax::Logger::getInstance().debug(message)

namespace vax {
class Logger {
  public:
    enum class Mode {
        CONSOLE,
        FILE,
        BOTH,
    };

    std::string system = "";

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    Logger() = default;

    Logger(const std::string system)
        : system(system) {};

    Logger(const std::string system, const std::string path)
        : system(system)
        , _fsLogger(std::make_shared<FsLogger>(path)) {};

    ~Logger() = default;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    Logger(Logger&&) = default;
    Logger& operator=(Logger&&) = default;

    template <typename... Args> void info(const Args&... args) const { log("INFO", args...); }

    template <typename... Args> void error(const Args&... args) const { log("ERROR", args...); }

    template <typename... Args> void warning(const Args&... args) const { log("WARNING", args...); }

    template <typename... Args> void debug(const Args&... args) const { log("DEBUG", args...); }

    void setMode(Mode mode) { _mode = mode; }

    Mode getMode() const { return _mode; }

    void setPath(const std::string& path) { _fsLogger = std::make_shared<FsLogger>(path); }

    void setFsLogger(std::shared_ptr<FsLogger> fsLogger) { _fsLogger = fsLogger; }

    std::shared_ptr<FsLogger> getFsLogger() const { return _fsLogger; }

  private:
    std::shared_ptr<FsLogger> _fsLogger;
    Mode _mode = Mode::CONSOLE;

    template <typename... Args> void log(std::string level, const Args&... args) const {
        std::ostringstream ss;
        ss << "[" << level << "] ";
        if (!system.empty()) {
            ss << "[" << system << "] ";
        }
        std::string separator = "";
        (((ss << separator << args), separator = ""), ...);
        if (_mode == Mode::CONSOLE || _mode == Mode::BOTH) {
            std::cout << ss.str() << std::endl;
        }
        if (_mode == Mode::FILE || _mode == Mode::BOTH) {
            if (_fsLogger) {
                _fsLogger->log(ss.str());
            } else {
                std::cerr << "No file path set for file logger." << std::endl;
            }
        }
    }
};
} // namespace vax