//
// Created by Fabian Kopf on 19.11.25.
//
#include <gtest/gtest.h>
#include "../wishlist.h"
#include "../wishlist_manager.h"
#include "../file_handler.h"
#include "../logger.h"
#include "test_helpers.h"
#include <filesystem>

class IntegrationTest : public ::testing::Test {
protected:
    std::string testFile = "test_integration.dat";

    void SetUp() override {
        Logger::getInstance().setLogLevel(LogLevel::NONE);
        std::filesystem::remove(testFile);
    }

    void TearDown() override {
        std::filesystem::remove(testFile);
    }
};

// ==================== End-to-End Workflow Tests ====================

TEST_F(IntegrationTest, CompleteWorkflow) {
    // 1. Create manager and add items
    WishlistManager manager("TestUser");
    TestHelpers::populateDiverseManager(manager);

    EXPECT_EQ(manager.getTotalItems(), 5);
    EXPECT_EQ(manager.getPurchasedCount(), 2);

    // 2. Save to file
    FileHandler handler(testFile);
    EXPECT_TRUE(handler.save(manager));

    // 3. Load into new manager
    WishlistManager loadedManager("TempUser");
    EXPECT_TRUE(handler.load(loadedManager));

    // 4. Verify loaded data
    EXPECT_EQ(loadedManager.getOwner(), "TestUser");
    EXPECT_EQ(loadedManager.getTotalItems(), 5);
    EXPECT_EQ(loadedManager.getPurchasedCount(), 2);

    // 5. Verify values
    EXPECT_DOUBLE_EQ(loadedManager.getTotalValue(),
                     19.99 + 14.99 + 999.99 + 29.99 + 24.99);
}

TEST_F(IntegrationTest, AddSearchModifySave) {
    WishlistManager manager("TestUser");

    // Add items
    manager.addItem(std::make_unique<WishItem>("PlayStation 5", 499.99, Category::ELECTRONICS));
    manager.addItem(std::make_unique<WishItem>("PlayStation Controller", 59.99, Category::ELECTRONICS));
    manager.addItem(std::make_unique<WishItem>("Book", 29.99, Category::BOOKS));

    // Search
    auto results = manager.findByName("PlayStation");
    EXPECT_EQ(results.size(), 2);

    // Modify
    if (!results.empty()) {
        results[0]->setPurchased(true);
    }

    // Save
    FileHandler handler(testFile);
    EXPECT_TRUE(handler.save(manager));

    // Reload and verify
    WishlistManager reloadedManager("Test");
    handler.load(reloadedManager);
    EXPECT_EQ(reloadedManager.getPurchasedCount(), 1);
}

TEST_F(IntegrationTest, SortAndFilter) {
    WishlistManager manager("TestUser");
    TestHelpers::populateDiverseManager(manager);

    // Sort by price descending
    manager.sort(SortOrder::BY_PRICE_DESC);
    const auto& items = manager.getItems();
    EXPECT_GT(items[0]->getPrice(), items[items.size() - 1]->getPrice());

    // Filter by category
    auto electronics = manager.findByCategory(Category::ELECTRONICS);
    EXPECT_EQ(electronics.size(), 1);
    EXPECT_EQ(electronics[0]->getName(), "Laptop");

    // Filter by price range
    auto midRange = manager.findByPriceRange(20.0, 50.0);
    EXPECT_GE(midRange.size(), 1);
}

TEST_F(IntegrationTest, BulkOperations) {
    WishlistManager manager("TestUser");
    TestHelpers::populateManager(manager, 10);

    // Mark all as purchased
    manager.markAllPurchased();
    EXPECT_EQ(manager.getPurchasedCount(), 10);

    // Clear purchased
    manager.clearAllPurchased();
    EXPECT_EQ(manager.getTotalItems(), 0);
}

TEST_F(IntegrationTest, MultipleManagersIndependence) {
    // Create two independent managers
    WishlistManager manager1("User1");
    WishlistManager manager2("User2");

    manager1.addItem(std::make_unique<WishItem>("Item1", 10.0));
    manager2.addItem(std::make_unique<WishItem>("Item2", 20.0));
    manager2.addItem(std::make_unique<WishItem>("Item3", 30.0));

    // They should be independent
    EXPECT_EQ(manager1.getTotalItems(), 1);
    EXPECT_EQ(manager2.getTotalItems(), 2);

    // Save both
    FileHandler handler1("test_user1.dat");
    FileHandler handler2("test_user2.dat");

    handler1.save(manager1);
    handler2.save(manager2);

    // Load into fresh managers
    WishlistManager loaded1("Temp1");
    WishlistManager loaded2("Temp2");

    handler1.load(loaded1);
    handler2.load(loaded2);

    EXPECT_EQ(loaded1.getTotalItems(), 1);
    EXPECT_EQ(loaded2.getTotalItems(), 2);

    // Cleanup
    std::filesystem::remove("test_user1.dat");
    std::filesystem::remove("test_user2.dat");
}

// ==================== Edge Case Tests ====================

TEST_F(IntegrationTest, LargeDataset) {
    WishlistManager manager("TestUser");

    // Add 1000 items
    for (int i = 0; i < 1000; ++i) {
        manager.addItem(std::make_unique<WishItem>(
            "Item " + std::to_string(i),
            10.0 + i
        ));
    }

    EXPECT_EQ(manager.getTotalItems(), 1000);

    // Save and load
    FileHandler handler(testFile);
    EXPECT_TRUE(handler.save(manager));

    WishlistManager loadedManager("Test");
    EXPECT_TRUE(handler.load(loadedManager));

    EXPECT_EQ(loadedManager.getTotalItems(), 1000);
}

TEST_F(IntegrationTest, UnicodeCharacters) {
    WishlistManager manager("用户"); // Chinese characters

    manager.addItem(std::make_unique<WishItem>("日本語アイテム", 100.0)); // Japanese
    manager.addItem(std::make_unique<WishItem>("Ελληνικά", 200.0)); // Greek
    manager.addItem(std::make_unique<WishItem>("Русский", 300.0)); // Russian

    FileHandler handler(testFile);
    EXPECT_TRUE(handler.save(manager));

    WishlistManager loadedManager("Test");
    EXPECT_TRUE(handler.load(loadedManager));

    EXPECT_EQ(loadedManager.getTotalItems(), 3);
}

TEST_F(IntegrationTest, ConcurrentOperations) {
    // Test that operations don't interfere with each other
    WishlistManager manager("TestUser");
    TestHelpers::populateManager(manager, 5);

    // Simulate concurrent-like operations
    auto item1 = manager.findById(1);
    auto item2 = manager.findById(2);

    if (item1) item1->setPurchased(true);
    if (item2) item2->setPrice(99.99);

    manager.sort(SortOrder::BY_PRICE_ASC);
    manager.addItem(std::make_unique<WishItem>("New Item", 5.0));

    EXPECT_EQ(manager.getTotalItems(), 6);
    EXPECT_EQ(manager.getPurchasedCount(), 1);
}