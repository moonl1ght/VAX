#ifndef Logger_hpp
#define Logger_hpp

#include <iostream>

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
    void log(const Args&... args) {
        std::cout << "[LOG 📝] ";
        (std::cout << ... << args) << std::endl;
    }

    template<typename... Args>
    void error(const Args&... args) {
        std::cout << "[ERROR 💥] ";
        (std::cerr << ... << args) << std::endl;
    }

    template<typename... Args>
    void debugPrint(const Args&... args) {
        std::cout << "[DEBUG 🐛] ";
        (std::cout << ... << args) << std::endl;
    }
};

#endif