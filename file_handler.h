//
// Created by Fabian Kopf on 14.11.25.
//

#ifndef CHISTMAS_WISHLIST_FILE_HANDLER_H
#define CHISTMAS_WISHLIST_FILE_HANDLER_H

#include <string>
#include "wishlist_manager.h"

class FileHandler {
private:
    std::string filename;

public:
    explicit FileHandler(const std::string &filename = "wishlist.dat");

    bool save(const WishlistManager &manager);

    bool load(WishlistManager &manager);

    bool exportToCSV(const WishlistManager &manager, const std::string &csvFile);

    bool importFromCSV(const WishlistManager &manager, const std::string &csvFile);
};

#endif //CHISTMAS_WISHLIST_FILE_HANDLER_H
