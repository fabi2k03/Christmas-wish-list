//
// Created by Fabian Kopf on 11.11.25.
//

#include "wishlist_manager.h"

#include <iostream>
#include <ostream>

WishlistManager::WishlistManager(const std::string &owner) : owner(owner) {
    std::cout << "[WishlistManger] Created for: " << owner << std::endl;
}

WishlistManager::~WishlistManager() {
    std::cout << "[WishlistManager] Destroying manager with: " << items.size() << " items" << std::endl;
}

void WishlistManager::addItem(std::unique_ptr<WishItem> item) {
    if (item) {
        std::cout << "[WishlistManager] Adding item: " << item->getName() << std::endl;
        items.push_back(std::move(item));
    }
}

bool WishlistManager::removeItem(int id) {
    auto it = std::find_if(items.begin(), items.end(), [id](const std::unique_ptr<WishItem> &item) {
        return item->getId() == id;
    });
    if (it != items.end()) {
        std::cout << "[WishlistManager] Removing item ID: " << id << std::endl;
        items.erase(it);
        return true;
    }
    return false;
}

WishItem *WishlistManager::findById(int id) {
    auto it = std::find_if(items.begin(), items.end(), [id](const std::unique_ptr<WishItem> &item) {
        return item->getId() == id;
    });
    return (it != items.end()) ? it->get() : nullptr;
}

std::vector<WishItem *> WishlistManager::findByName(const std::string &name) {
    std::vector<WishItem *> result;
    for (auto &item: items) {
        if (item->getName().find(name) != std::string::npos) {
            result.push_back(item.get());
        }
    }
    return result;
}

std::vector<WishItem *> WishlistManager::findByCategory(Category cat) {
    return filter([cat](const WishItem &item) {
        return item.getCategory() == cat;
    });
}

std::vector<WishItem *> WishlistManager::findByPriceRange(double min, double max) {
    return filter([min, max](const WishItem &item) {
        return item.getPrice() >= min && item.getPrice() <= max;
    });
}



