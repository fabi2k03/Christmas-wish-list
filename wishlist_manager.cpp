//
// Created by Fabian Kopf on 11.11.25.
//

#include "wishlist_manager.h"
#include "logger.h"

#include <iostream>
#include <map>
#include <ostream>
#include <iomanip>

WishlistManager::WishlistManager(const std::string &owner) : owner(owner) {
    LOG_INFO("[WishlistManger] Created for: " , owner);
}

WishlistManager::~WishlistManager() {
    LOG_INFO("[WishlistManager] Destroying manager with: " , items.size() ," items");
}

void WishlistManager::addItem(std::unique_ptr<WishItem> item) {
    if (item) {
        LOG_INFO("[WishlistManager] Adding item: " , item->getName());
        items.push_back(std::move(item));
    }
}

bool WishlistManager::removeItem(int id) {
    auto it = std::find_if(items.begin(), items.end(), [id](const std::unique_ptr<WishItem> &item) {
        return item->getId() == id;
    });
    if (it != items.end()) {
        LOG_INFO("[WishlistManager] Removing item ID: " , id);
        items.erase(it);
        return true;
    }
    LOG_WARNING("WishlistManager: Item ID ", id, " not found for removal");
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

std::vector<WishItem *> WishlistManager::filter(std::function<bool(const WishItem &)> predicate) {
    std::vector<WishItem *> result;
    for (auto &item: items) {
        if (predicate(*item)) {
            result.push_back(item.get());
        }
    }
    return result;
}

void WishlistManager::markAllPurchased() {
    for (auto &item: items) {
        item->setPurchased(true);
    }
    std::cout << "[WishlistManager] Marked all items as purchased" << std::endl;
}

void WishlistManager::clearAllPurchased() {
    auto it = std::remove_if(items.begin(), items.end(), [](const std::unique_ptr<WishItem> &item) {
        return item->isPurchased();
    });
    items.erase(it, items.end());
    std::cout << "[WishlistManager] Cleared purchased items" << std::endl;
}

void WishlistManager::sort(SortOrder order) {
    switch (order) {
        case SortOrder::BY_PRIORITY:
            std::sort(items.begin(), items.end(), [](const auto &a, const auto &b) { return *a < *b; });
            break;
        case SortOrder::BY_PRICE_ASC:
            std::sort(items.begin(), items.end(), [](const auto &a, const auto &b) {
                return a->getPrice() < b->getPrice();
            });
            break;
        case SortOrder::BY_PRICE_DESC:
            std::sort(items.begin(), items.end(), [](const auto &a, const auto &b) {
                return a->getPrice() > b->getPrice();
            });
            break;
        case SortOrder::BY_NAME:
            std::sort(items.begin(), items.end(),
                      [](const auto &a, const auto &b) { return a->getName() < b->getName(); });
            break;
        case SortOrder::BY_CATEGORY:
            std::sort(items.begin(), items.end(), [](const auto &a, const auto &b) {
                return a->getCategory() < b->getCategory();
            });
            break;
        case SortOrder::BY_ID:
            std::sort(items.begin(), items.end(), [](const auto &a, const auto &b) { return a->getId() < b->getId(); });
            break;
    }
}

int WishlistManager::getTotalItems() const {
    return items.size();
}

int WishlistManager::getPurchasedCount() const {
    return std::count_if(items.begin(), items.end(), [](const auto &item) { return item->isPurchased(); });
}

double WishlistManager::getTotalValue() const {
    double total = 0.0;
    for (const auto &item: items) {
        total += item->getPrice();
    }
    return total;
}

double WishlistManager::getPurchasedValue() const {
    double total = 0.0;
    for (const auto &item: items) {
        if (item->isPurchased()) {
            total += item->getPrice();
        }
    }
    return total;
}

double WishlistManager::getRemainingValue() const {
    return getTotalValue() - getPurchasedValue();
}

void WishlistManager::displayAll() const {
    if (items.empty()) {
        std::cout << "\n Wishlist is empty!" << std::endl;
        return;
    }

    std::cout << "\n=== ALL ITEMS ===\n";
    for (const auto &item: items) {
        std::cout << *item << " - " << (item->isPurchased() ? "Purchased" : "Pending") << "\n";
    }
}

void WishlistManager::displayPending() const {
    std::cout << "\n=== PENDING ITEMS ===\n";
    for (const auto &item: items) {
        if (!item->isPurchased()) {
            std::cout << *item << "\n";
        }
    }
}

void WishlistManager::displayPurchased() const {
    std::cout << "\n=== PURCHASED ITEMS ===\n";
    for (const auto &item: items) {
        if (item->isPurchased()) {
            std::cout << *item << "\n";
        }
    }
}

void WishlistManager::displayByCategory() const {
    std::map<Category, std::vector<WishItem *> > byCategory;

    for (const auto &item: items) {
        byCategory[item->getCategory()].push_back(item.get());
    }

    std::cout << "\n=== ITEMS BY CATEGORY ===\n";
    for (const auto &[cat, items]: byCategory) {
        std::cout << "\n" << WishItem::categoryToString(cat) << ":\n";
        for (const auto &item: items) {
            std::cout << "   " << *item << "\n";
        }
    }
}

void WishlistManager::displayStatistics() const {
    std::cout << "\n=== STATISTICS ===\n";
    std::cout << "Owner: " << owner << "\n";
    std::cout << "Total Items: " << getTotalItems() << "\n";
    std::cout << "Purchased: " << getPurchasedCount() << "\n";
    std::cout << "Pending: " << (getTotalItems() - getPurchasedCount()) << "\n";
    std::cout << "Total Value: $" << std::fixed << std::setprecision(2) << getTotalValue() << "\n";
    std::cout << "Purchased Value: $" << getPurchasedValue() << "\n";
    std::cout << "Remaining Value: $" << getRemainingValue() << "\n";
}
