//
// Created by Fabian Kopf on 14.11.25.
//

#include "utils.h"

#include <iostream>
#include <limits>

namespace Utlis {
    void clearInput() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    void clearOutput() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void pause() {
        std::cout << "\nPress Enter key to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    int getIntInput(const std::string &prompt, int min, int max) {
        int value;
        while (true) {
            std::cout << prompt;
            std::cin >> value;

            if (std::cin.fail() || value < min || value > max) {
                clearInput();
                std::cout << "Invalid input! Please enter a number between " << min << " and " << max << "\n.";
            } else {
                clearInput();
                return value;
            }
        }
    }

    double getDoubleInput(const std::string &prompt, double min) {
        double value;
        while (true) {
            std::cout << prompt;
            std::cin >> value;

            if (std::cin.fail() || value < min) {
                clearInput();
                std::cout << "Invalid input! Please enter a number >= " << min << ".\n";
            } else {
                clearInput();
                return value;
            }
        }
    }

    std::string getStringInput(const std::string &prompt) {
        std::string input;
        std::cout << prompt;
        std::getline(std::cin, input);
        return input;
    }

    bool confirm(const std::string &message) {
        std::cout << message << "(y/n): ";
        char choice;
        std::cin >> choice;
        clearInput();
        return (choice == 'y' || choice == 'Y');
    }
}
