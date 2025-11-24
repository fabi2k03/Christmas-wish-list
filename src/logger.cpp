//
// Created by Fabian Kopf on 18.11.25.
//

#include "../include/logger.h"

Logger *Logger::instance = nullptr;

Logger::Logger() : currentLevel(LogLevel::INFO), consoleOutput(false) {
    logFile.open("wishlist_app.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Warning: Could not open log file!" << std::endl;
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

Logger &Logger::getInstance() {
    if (instance == nullptr) {
        instance = new Logger();
    }
    return *instance;
}

std::string Logger::getCurrentTime() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

void Logger::setLogLevel(LogLevel level) {
    currentLevel = level;
}

void Logger::enableConsoleOutput(bool enable) {
    consoleOutput = enable;
}

void Logger::setLogFile(const std::string &filename) {
    std::lock_guard<std::mutex> lock(logMutex);

    if (logFile.is_open()) {
        logFile.close();
    }

    logFile.open(filename, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Warning: Could not open log file: " << filename << std::endl;
    }
}

void Logger::log(LogLevel level, const std::string &message) {
    if (level < currentLevel || level == LogLevel::NONE) {
        return;
    }
    std::lock_guard<std::mutex> lock(logMutex);

    std::string timestamp = getCurrentTime();
    std::string levelString = levelToString(level);
    std::string logMessage = "[" + timestamp + "] [" + levelString + "] " + message;

    //Write to File
    if (logFile.is_open()) {
        logFile << logMessage << std::endl;
        logFile.flush();
    }

    if (consoleOutput) {
        if (level == LogLevel::ERROR) {
            std::cerr << logMessage << std::endl;
        }else {
            std::cout << logMessage << std::endl;
        }
    }
}

void Logger::debug(const std::string &message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string &message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string &message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string &message) {
    log(LogLevel::ERROR, message);
}