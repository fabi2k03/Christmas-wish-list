//
// Created by Fabian Kopf on 19.11.25.
//
#include <gtest/gtest.h>
#include "../include/logger.h"

int main(int argc, char **argv) {
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);

    // Disable logging during tests
    Logger::getInstance().setLogLevel(LogLevel::NONE);
    Logger::getInstance().enableConsoleOutput(false);

    // Run all tests
    return RUN_ALL_TESTS();
}