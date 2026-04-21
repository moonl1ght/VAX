#pragma once

#include <iostream>

#define LOG_INFO(message) vax::utils::Logger::getInstance().info(message)
#define LOG_ERROR(message) vax::utils::Logger::getInstance().error(message)
#define LOG_WARNING(message) vax::utils::Logger::getInstance().warning(message)
#define LOG_DEBUG(message) vax::utils::Logger::getInstance().debug(message)

namespace vax::utils {
    class Logger {
    public:
        std::string system = "";

        static Logger& getInstance() {
            static Logger instance;
            return instance;
        }

        Logger(const std::string system) : system(system) {};
        Logger() = default;
        ~Logger() = default;

        Logger(const Logger&) = default;
        Logger& operator=(const Logger&) = default;

        Logger(Logger&&) = default;
        Logger& operator=(Logger&&) = default;

        template<typename... Args>
        void info(const Args&... args) const {
            log("INFO", args...);
        }

        template<typename... Args>
        void error(const Args&... args) const {
            log("ERROR", args...);
        }

        template<typename... Args>
        void warning(const Args&... args) const {
            log("WARNING", args...);
        }

        template<typename... Args>
        void debug(const Args&... args) const {
            log("DEBUG", args...);
        }

    private:

        template<typename... Args>
        void log(std::string level, const Args&... args) const {
            std::cout << "[" << level << "] ";
            if (!system.empty()) {
                std::cout << "[" << system << "] ";
            }
            (std::cout << ... << args) << std::endl;
        }
    };
}