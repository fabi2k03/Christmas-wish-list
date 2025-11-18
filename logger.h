//
// Created by Fabian Kopf on 18.11.25.
//

#ifndef CHISTMAS_WISHLIST_LOGGER_H
#define CHISTMAS_WISHLIST_LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <sstream>

enum class LogLevel {
    DEBUG, INFO, WARNING, ERROR, NONE
};

class Logger {
private:
    static Logger* instance;
    std::ofstream logFile;
    LogLevel currentLevel;
    std::mutex logMutex;
    bool consoleOutput;

    Logger();

    std::string getCurrentTime();

    std::string levelToString(LogLevel level);

public:
    static Logger &getInstance();

    ~Logger();

    Logger(const Logger &) = delete;

    Logger &operator=(const Logger &) = delete;

    void setLogLevel(LogLevel level);

    void enableConsoleOutput(bool enable);

    void setLogFile(const std::string &filename);

    void log(LogLevel level, const std::string &message);

    void debug(const std::string &message);

    void info(const std::string &message);

    void warning(const std::string &message);

    void error(const std::string &message);

    template<typename... Args>
    void debug(Args... args) {
        log(LogLevel::DEBUG, concatenate(args...));
    }

    template<typename... Args>
    void info(Args... args) {
        log(LogLevel::INFO, concatenate(args...));
    }

    template<typename... Args>
    void warning(Args... args) {
        log(LogLevel::WARNING, concatenate(args...));
    }

    template<typename... Args>
    void error(Args... args) {
        log(LogLevel::ERROR, concatenate(args...));
    }

private:
    template<typename T>
    std::string toString(T value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    template<typename T>
    std::string concatenate(T value) {
        return toString(value);
    }

    template<typename T, typename... Args>
    std::string concatenate(T first, Args... args) {
        return toString(first) + concatenate(args...);
    }
};

// Global convenience macros
#define LOG_DEBUG(...) logger::getInstance().debug(__VA_ARGS__)
#define LOG_INFO(...) logger::getInstance().info(__VA_ARGS__)
#define LOG_WARNING(...) logger::getInstance().warning(__VA_ARGS__)
#define LOG_ERROR(...) logger::getInstance().error(__VA_ARGS__)


#endif //CHISTMAS_WISHLIST_LOGGER_H
