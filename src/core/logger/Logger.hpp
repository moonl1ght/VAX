#ifndef Logger_hpp
#define Logger_hpp

#include <iostream>

#define LOG_INFO(message) Logger::getInstance().log(message)
#define LOG_ERROR(message) Logger::getInstance().error(message)
#define LOG_WARNING(message) Logger::getInstance().warning(message)
#define LOG_DEBUG(message) Logger::getInstance().debugPrint(message)

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    Logger() = default;
    ~Logger() = default;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    template<typename... Args>
    void log(const Args&... args) const {
        std::cout << "[LOG] ";
        (std::cout << ... << args) << std::endl;
    }

    template<typename... Args>
    void error(const Args&... args) const {
        std::cout << "[ERROR] ";
        (std::cerr << ... << args) << std::endl;
    }

    template<typename... Args>
    void warning(const Args&... args) const {
        std::cout << "[WARNING] ";
        (std::cout << ... << args) << std::endl;
    }

    template<typename... Args>
    void debugPrint(const Args&... args) const {
        std::cout << "[DEBUG] ";
        (std::cout << ... << args) << std::endl;
    }
};

#endif