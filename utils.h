//
// Created by Fabian Kopf on 14.11.25.
//

#ifndef CHISTMAS_WISHLIST_UTILS_H
#define CHISTMAS_WISHLIST_UTILS_H

#include <string>
#include <climits>

namespace Utils {
    void clearInput();
    void clearScreen();
    void pause();
    int getIntInput(const std::string &prompt, int min = INT_MIN, int max = INT_MAX);
    double getDoubleInput(const std::string &prompt, double min = 0.0);
    std::string getStringInput(const std::string &prompt);
    bool confirm(const std::string &message);
}

#endif //CHISTMAS_WISHLIST_UTILS_H