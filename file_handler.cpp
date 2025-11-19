//
// Created by Fabian Kopf on 14.11.25.
//

#include "file_handler.h"
#include "logger.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

FileHandler::FileHandler(const std::string &filename) : filename(filename) {
    LOG_INFO("[FileHandler]: Initialized with filename " , filename);
}

bool FileHandler::save(const WishlistManager &manager) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        LOG_ERROR("[FileHandler]: Could not open file for writing: " , filename);
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }

    file << manager.getOwner() << "\n";

    LOG_DEBUG("[FileHandler] DEBUG: Saved owner: " , manager.getOwner());

    for (const auto &item: manager.getItems()) {
        file << item->serialize() << "\n";
    }

    file.close();
    LOG_INFO("FileHandler: Successfully saved ", manager.getTotalItems(), " items");
    std::cout << "Wishlist saved successfully!\n";
    return true;
}
bool FileHandler::load(WishlistManager &manager) {
    LOG_INFO("FileHandler: Attempting to load from '", filename, "'");

    std::ifstream file(filename);
    if (!file.is_open()) {
        LOG_WARNING("FileHandler: File '", filename, "' not found or could not be opened");
        std::cerr << "Warning: Could not open file for reading: " << filename << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        LOG_INFO("FileHandler: File is empty, starting fresh");
        std::cout << "Starting fresh wishlist!\n";
        file.close();
        return false;
    }
    file.seekg(0, std::ios::beg);

    std::string firstLine;
    std::getline(file, firstLine);

    LOG_DEBUG("FileHandler: First line read - '", firstLine, "'");

    int count = 0;

    // Skip empty lines at the beginning
    while (firstLine.empty() && std::getline(file, firstLine)) {
        LOG_DEBUG("FileHandler: Skipping empty line");
    }

    if (firstLine.empty()) {
        LOG_WARNING("FileHandler: File contains only empty lines");
        file.close();
        return false;
    }

    if (firstLine.find('|') != std::string::npos) {
        // First line is an item - keep current owner
        LOG_INFO("FileHandler: Old format detected (no owner line), keeping current owner");

        // Process first line as item
        auto item = WishItem::deserialize(firstLine);
        if (item) {
            manager.addItem(std::move(item));
            count++;
        }
    } else {
        // First line is owner
        if (!firstLine.empty()) {
            LOG_INFO("FileHandler: Setting owner from file - '", firstLine, "'");
            manager.setOwner(firstLine);
        }
    }

    // Read remaining lines
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        LOG_DEBUG("FileHandler: Processing line - '", line, "'");

        auto item = WishItem::deserialize(line);
        if (item) {
            manager.addItem(std::move(item));
            count++;
        }
    }

    file.close();
    LOG_INFO("FileHandler: Successfully loaded ", count, " items for owner '", manager.getOwner(), "'");
    std::cout << "Loaded " << count << " item(s) from wishlist\n";
    return count > 0;
}

bool FileHandler::exportToCSV(const WishlistManager &manager, const std::string &csvFile) {
    std::ofstream file(csvFile);
    if (!file.is_open()) {
        LOG_ERROR("Error: Could not open file for writing: " , csvFile );
        return false;
    }

    file << "ID,Name,Price,Purchased,Category,Priority,Notes,Link\n";

    for (const auto &item: manager.getItems()) {
        file << item->getId() << ","
                << item->getName() << ","
                << item->getPrice() << ","
                << (item->isPurchased() ? "Yes" : "No") << ","
                << WishItem::categoryToString(item->getCategory()) << ","
                << WishItem::priorityToString(item->getPriority()) << ","
                << item->getNotes() << ","
                << item->getLink() << "\n";
    }

    file.close();
    LOG_DEBUG("[FileHandler] Exported to CSV", csvFile);
    std::cout << "Wishlist exported to CSV successfully!\n";
    return true;
}

 bool FileHandler::importFromCSV(WishlistManager &manager, const std::string &csvFile) {
    std::ifstream file(csvFile);

    if (!file.is_open()) {
        LOG_ERROR("Error: Could not open CSV file: " , csvFile);
        return false;
    }

    std::string line;
    int count = 0;
    bool isFirstLine = true;

    // Read file line by line
    while (std::getline(file, line)) {
        // Skip header line
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }

        // Skip empty lines
        if (line.empty()) continue;

        // Parse CSV line
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;

        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 4) {
            LOG_WARNING("Warning: Skipping invalid line: " , line);
            std::cerr << "Warning: Skipping invalid line: " << line << std::endl;
            continue;
        }

        try {
            // Create new item
            auto item = std::make_unique<WishItem>();

            // Parse fields (skip ID - let it auto-generate)
            // CSV format: ID,Name,Price,Purchased,Category,Priority,Notes,Link
            item->setName(tokens[1]); // Name
            item->setPrice(std::stod(tokens[2])); // Price

            // Purchased status
            std::string purchasedStr = tokens[3];
            std::transform(purchasedStr.begin(), purchasedStr.end(),
                           purchasedStr.begin(), ::tolower);
            item->setPurchased(purchasedStr == "yes" || purchasedStr == "1" || purchasedStr == "true");

            // Category
            if (tokens.size() > 4) {
                item->setCategory(WishItem::stringToCategory(tokens[4]));
            }

            // Priority
            if (tokens.size() > 5) {
                item->setPriority(WishItem::stringToPriority(tokens[5]));
            }

            // Notes
            if (tokens.size() > 6 && !tokens[6].empty()) {
                item->setNotes(tokens[6]);
            }

            // Link
            if (tokens.size() > 7 && !tokens[7].empty()) {
                item->setLink(tokens[7]);
            }

            // Add to manager
            manager.addItem(std::move(item));
            count++;
        } catch (const std::exception &e) {
            LOG_WARNING("Warning: Error parsing line: " , line , " (" , e.what() , ")");
            std::cerr << "Warning: Error parsing line: " << line << " (" << e.what() << ")" << std::endl;
            continue;
        }
    }

    file.close();
    LOG_INFO("[FileHandler] Imported " , count , " items from CSV: " , csvFile);
    std::cout << "Imported " << count << " item(s) from CSV successfully!\n";

    return count > 0;
}
