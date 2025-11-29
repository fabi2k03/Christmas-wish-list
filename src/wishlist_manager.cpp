//
// Created by Fabian Kopf on 11.11.25.
//

#include "../include/wishlist_manager.h"
#include "../include/logger.h"
#include "../include/database_handler.h"

#include <iostream>
#include <map>
#include <ostream>
#include <iomanip>

WishlistManager::WishlistManager(const std::string &owner) : owner(owner), dbHandler(nullptr) {
    LOG_INFO("[WishlistManger] Created for: ", owner);
}

WishlistManager::~WishlistManager() {
    LOG_INFO("[WishlistManager] Destroying manager with: ", items.size(), " items");
}

void WishlistManager::addItem(std::unique_ptr<WishItem> item) {
    if (item) {
        LOG_INFO("[WishlistManager] Adding item: ", item->getName());
        items.push_back(std::move(item));
        if (dbHandler) {
            dbHandler->saveItem(*items.back(), owner);
        }
    }
}

bool WishlistManager::removeItem(int id) {
    auto it = std::find_if(items.begin(), items.end(), [id](const std::unique_ptr<WishItem> &item) {
        return item->getId() == id;
    });
    if (it != items.end()) {
        LOG_INFO("[WishlistManager] Removing item ID: ", id);
        if (dbHandler) dbHandler->deleteItem(id, owner);
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

void WishlistManager::setBudget(double amount) {
    budget.setMaxBudget(amount);
    if (dbHandler) dbHandler->saveBudget(budget, owner);
    syncBudgetWithPurchases();
    LOG_INFO("WishlistManager: Budget set to ", amount, " for user ", owner);

}

Budget &WishlistManager::getBudget() {
    return budget;
}

const Budget &WishlistManager::getBudget() const {
    return budget;
}

void WishlistManager::enableBudget() {
    budget.enable();
    syncBudgetWithPurchases();
    LOG_INFO("WishlistManager: Budget enabled for user, ", owner);
}

void WishlistManager::disableBudget() {
    budget.disable();
    LOG_INFO("WishlistManager: Budget disabled for user, ", owner);
}

void WishlistManager::resetBudget() {
    budget.reset();
    syncBudgetWithPurchases();
    LOG_INFO("WishlistManager: Budget reset for user ", owner);
}

void WishlistManager::displayBudgetStatus() const {
    if (!budget.isEnabled()) {
        std::cout << "\n📊 Budget tracking is currently disabled.\n";
        std::cout << "   Enable it in the Budget menu to track spending.\n";
        return;
    }

    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║         💰 BUDGET STATUS 💰         ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Max Budget:     €" << budget.getMaxBudget() << "\n";
    std::cout << "Spent:          €" << budget.getSpentAmount() << "\n";
    std::cout << "Remaining:      €" << budget.getRemaining() << "\n";
    std::cout << "Used:           " << budget.getSpendingPercentage() << "%\n";
    std::cout << "\nStatus:         " << budget.getStatusMessage() << "\n";

    // Visual progress bar
    int barWidth = 40;
    int progress = static_cast<int>(budget.getSpendingPercentage() / 100.0 * barWidth);
    if (progress > barWidth) progress = barWidth;

    std::cout << "\nProgress:       [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < progress) {
            if (budget.isOverBudget()) {
                std::cout << "█"; // Red/full if over budget
            } else if (budget.isNearLimit(0.8)) {
                std::cout << "▓"; // Warning
            } else {
                std::cout << "▓"; // Normal
            }
        } else {
            std::cout << "░";
        }
    }
    std::cout << "]\n";

    // Breakdown by category
    if (!items.empty()) {
        std::cout << "\n--- Spending by Category ---\n";
        std::map<Category, double> categorySpending;

        for (const auto &item: items) {
            if (item->isPurchased()) {
                categorySpending[item->getCategory()] += item->getPrice();
            }
        }

        for (const auto &[cat, amount]: categorySpending) {
            std::cout << "  " << WishItem::categoryToString(cat) << ": €" << amount << "\n";
        }
    }

    std::cout << "\n";
}

bool WishlistManager::checkBudgetBevorAdd(double price) const {
    if (!budget.isEnabled()) return true;

    double potentialSpent = budget.getSpentAmount() + price;
    double potentialRemaining = budget.getMaxBudget() - potentialSpent;

    if (potentialSpent > budget.getMaxBudget()) {
        std::cout << "\nBUDGET WARNING!\n";
        std::cout << "Adding this item would exceed your budget:\n";
        std::cout << "  Current spent:    €" << std::fixed << std::setprecision(2) << budget.getSpentAmount() << "\n";
        std::cout << "  Item price:       €" << price << "\n";
        std::cout << "  Would be:         €" << potentialSpent << "\n";
        std::cout << "  Budget:           €" << budget.getMaxBudget() << "\n";
        std::cout << "  Over by:          €" << (potentialSpent - budget.getMaxBudget()) << "\n";
        return false;
    }

    if (potentialRemaining < budget.getMaxBudget() * 0.2) {
        std::cout << "\n Budget Notice: Only €" << potentialRemaining << " remaining after this purchase. \n";
    }
    return true;
}

void WishlistManager::syncBudgetWithPurchases() {
    if (!budget.isEnabled()) return;

    double totalPurchased = getPurchasedValue();
    budget.setSpentAmount(totalPurchased);

    LOG_DEBUG("WishlistManager: Synced budget with purchases. Spent: ", totalPurchased);
}

void WishlistManager::updateBudgetFromItems() {
    syncBudgetWithPurchases();
}

void WishlistManager::setDatabaseHandler(DatabaseHandler *handler) {
    dbHandler = handler;
    LOG_INFO("WishlistManager: Database handler set for user: ", owner);
}

bool WishlistManager::saveToDatabase() {
    if (!dbHandler) {
        LOG_ERROR("WishlistManager: No database handler set");
        return false;
    }

    //Save all items
    for (const auto &item: items) {
        if (!dbHandler->saveItem(*item, owner)) {
            LOG_ERROR("WishlistManager: Failed to save item: ", item->getName());
            return false;
        }
    }

    //Save budget
    if (!dbHandler->saveBudget(budget, owner)) {
        LOG_ERROR("WishlistManager: Failed to save budget");
        return false;
    }

    LOG_INFO("WishlistManager: Successfully saved all data to database");
    return true;
}

bool WishlistManager::loadFromDatabase() {
    if (!dbHandler) {
        LOG_ERROR("WishlistManager: No database handler set");
        return false;
    }

    items.clear();

    int globalMaxId = dbHandler->getGlobalMaxItemId();

    if (globalMaxId > 0) {
        WishItem::setNextId(globalMaxId + 1);
        LOG_INFO("WishlistManager: Synchronized next item ID to: ", globalMaxId + 1, " (based on global max).");
    } else {
        WishItem::setNextId(1);
        LOG_INFO("WishlistManager: No items globally, ID counter set to 1.");
    }

    auto loadedItems = dbHandler->loadItems(owner);
    for (auto &item: loadedItems) {

        items.push_back(std::move(item));
    }

    budget = dbHandler->loadBudget(owner);

    LOG_INFO("WishlistManager: Successfully loaded data from database");
    return true;
}
