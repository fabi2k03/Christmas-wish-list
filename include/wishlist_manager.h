//
// Created by Fabian Kopf on 11.11.25.
//

#ifndef CHISTMAS_WISHLIST_WISHLIST_MANAGER_H
#define CHISTMAS_WISHLIST_WISHLIST_MANAGER_H

#include <vector>
#include <memory>
#include <functional>

#include "wishlist.h"
#include "budget.h"
class DatabaseHandler;

enum class SortOrder {
    BY_PRIORITY,
    BY_PRICE_ASC,
    BY_PRICE_DESC,
    BY_NAME,
    BY_CATEGORY,
    BY_ID
};

class WishlistManager {
private:
    std::vector<std::unique_ptr<WishItem>> items;
    std::string owner;
    Budget budget;
    DatabaseHandler *dbHandler;

    void updateBudgetFromItems();

public:
    explicit WishlistManager(const std::string &owner = "Default");
    ~WishlistManager();

    //Add & Remove
    void addItem(std::unique_ptr<WishItem> item);
    bool removeItem(int id);

    // Search
    WishItem* findById(int id);
    std::vector<WishItem*> findByName(const std::string &name);
    std::vector<WishItem*> findByCategory(Category cat);
    std::vector<WishItem*> findByPriceRange(double min, double max);
    std::vector<WishItem*> filter(std::function<bool(const WishItem&)> predicate);

    //Bulk operations
    void markAllPurchased();
    void clearAllPurchased();
    void sort(SortOrder order);

    //Statistics
    int getTotalItems() const;
    int getPurchasedCount() const;
    double getTotalValue() const;
    double getPurchasedValue() const;
    double getRemainingValue() const;

    //Display
    void displayAll() const;
    void displayPending() const;
    void displayPurchased() const;
    void displayByCategory() const;
    void displayStatistics() const;

    //Getters
    const std::vector<std::unique_ptr<WishItem>>& getItems() const {
        return items;
    }

    std::string getOwner() const {
        return owner;
    }

    void setOwner(const std::string &owner) {
        this->owner = owner;
    }

    void setBudget(double amount);
    Budget &getBudget();
    const Budget &getBudget() const;
    void enableBudget();
    void disableBudget();
    void resetBudget();
    void displayBudgetStatus() const;
    bool checkBudgetBevorAdd(double price) const;
    void syncBudgetWithPurchases();

    //Database methods
    void setDatabaseHandler(DatabaseHandler *handler);
    bool saveToDatabase();
    bool loadFromDatabase();
};

#endif //CHISTMAS_WISHLIST_WISHLIST_MANAGER_H