//
// Created by Fabian Kopf on 19.11.25.
//
#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <memory>
#include "../wishlist.h"
#include "../wishlist_manager.h"

namespace TestHelpers {
    // Helper to create a test item with default values
    inline std::unique_ptr<WishItem> createTestItem(
        const std::string &name = "Test Item",
        double price = 10.0,
        Category category = Category::OTHER,
        Priority priority = Priority::MEDIUM,
        bool purchased = false
    ) {
        auto item = std::make_unique<WishItem>(name, price, category);
        item->setPriority(priority);
        item->setPurchased(purchased);
        return item;
    }

    // Helper to populate a manager with test data
    inline void populateManager(WishlistManager &manager, int count = 5) {
        for (int i = 0; i < count; ++i) {
            manager.addItem(createTestItem(
                "Item " + std::to_string(i),
                10.0 * (i + 1)
            ));
        }
    }

    // Helper to create diverse test items
    inline void populateDiverseManager(WishlistManager &manager) {
        manager.addItem(createTestItem("Toy Car", 19.99, Category::TOYS, Priority::LOW, false));
        manager.addItem(createTestItem("Novel", 14.99, Category::BOOKS, Priority::MEDIUM, true));
        manager.addItem(createTestItem("Laptop", 999.99, Category::ELECTRONICS, Priority::URGENT, false));
        manager.addItem(createTestItem("T-Shirt", 29.99, Category::CLOTHING, Priority::LOW, true));
        manager.addItem(createTestItem("Soccer Ball", 24.99, Category::SPORTS, Priority::HIGH, false));
    }
} // namespace TestHelpers

#endif
