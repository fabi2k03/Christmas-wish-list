//
// Created by Fabian Kopf on 19.11.25.
//
#include <gtest/gtest.h>
#include "../include/wishlist_manager.h"
#include "../include/logger.h"

class WishlistManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::getInstance().setLogLevel(LogLevel::NONE);
    }

    void TearDown() override {
        // Cleanup
    }
};

// ==================== Constructor Tests ====================

TEST_F(WishlistManagerTest, Constructor) {
    WishlistManager manager("TestUser");

    EXPECT_EQ(manager.getOwner(), "TestUser");
    EXPECT_EQ(manager.getTotalItems(), 0);
}

// ==================== Add/Remove Tests ====================

TEST_F(WishlistManagerTest, AddItem) {
    WishlistManager manager("TestUser");

    auto item = std::make_unique<WishItem>("Test", 10.0);
    manager.addItem(std::move(item));

    EXPECT_EQ(manager.getTotalItems(), 1);
}

TEST_F(WishlistManagerTest, AddMultipleItems) {
    WishlistManager manager("TestUser");

    manager.addItem(std::make_unique<WishItem>("Item1", 10.0));
    manager.addItem(std::make_unique<WishItem>("Item2", 20.0));
    manager.addItem(std::make_unique<WishItem>("Item3", 30.0));

    EXPECT_EQ(manager.getTotalItems(), 3);
}

TEST_F(WishlistManagerTest, AddNullItem) {
    WishlistManager manager("TestUser");

    manager.addItem(nullptr);

    EXPECT_EQ(manager.getTotalItems(), 0);
}

TEST_F(WishlistManagerTest, RemoveExistingItem) {
    WishlistManager manager("TestUser");

    auto item = std::make_unique<WishItem>("Test", 10.0);
    int id = item->getId();
    manager.addItem(std::move(item));

    EXPECT_TRUE(manager.removeItem(id));
    EXPECT_EQ(manager.getTotalItems(), 0);
}

TEST_F(WishlistManagerTest, RemoveNonExistentItem) {
    WishlistManager manager("TestUser");

    manager.addItem(std::make_unique<WishItem>("Test", 10.0));

    EXPECT_FALSE(manager.removeItem(9999));
    EXPECT_EQ(manager.getTotalItems(), 1);
}

// ==================== Find Tests ====================

TEST_F(WishlistManagerTest, FindByIdExists) {
    WishlistManager manager("TestUser");

    auto item = std::make_unique<WishItem>("Test", 10.0);
    int id = item->getId();
    manager.addItem(std::move(item));

    WishItem* found = manager.findById(id);

    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getName(), "Test");
}

TEST_F(WishlistManagerTest, FindByIdNotExists) {
    WishlistManager manager("TestUser");

    manager.addItem(std::make_unique<WishItem>("Test", 10.0));

    WishItem* found = manager.findById(9999);

    EXPECT_EQ(found, nullptr);
}

TEST_F(WishlistManagerTest, FindByNameExactMatch) {
    WishlistManager manager("TestUser");

    manager.addItem(std::make_unique<WishItem>("PlayStation 5", 499.99));
    manager.addItem(std::make_unique<WishItem>("Book", 29.99));

    auto results = manager.findByName("PlayStation 5");

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0]->getName(), "PlayStation 5");
}

TEST_F(WishlistManagerTest, FindByNamePartialMatch) {
    WishlistManager manager("TestUser");

    manager.addItem(std::make_unique<WishItem>("PlayStation 5", 499.99));
    manager.addItem(std::make_unique<WishItem>("PlayStation Controller", 59.99));
    manager.addItem(std::make_unique<WishItem>("Book", 29.99));

    auto results = manager.findByName("PlayStation");

    EXPECT_EQ(results.size(), 2);
}

TEST_F(WishlistManagerTest, FindByNameNoMatch) {
    WishlistManager manager("TestUser");

    manager.addItem(std::make_unique<WishItem>("Test", 10.0));

    auto results = manager.findByName("NonExistent");

    EXPECT_EQ(results.size(), 0);
}

TEST_F(WishlistManagerTest, FindByCategory) {
    WishlistManager manager("TestUser");

    auto item1 = std::make_unique<WishItem>("Book1", 20.0, Category::BOOKS);
    auto item2 = std::make_unique<WishItem>("Book2", 30.0, Category::BOOKS);
    auto item3 = std::make_unique<WishItem>("Toy", 40.0, Category::TOYS);

    manager.addItem(std::move(item1));
    manager.addItem(std::move(item2));
    manager.addItem(std::move(item3));

    auto results = manager.findByCategory(Category::BOOKS);

    EXPECT_EQ(results.size(), 2);
}

TEST_F(WishlistManagerTest, FindByPriceRange) {
    WishlistManager manager("TestUser");

    manager.addItem(std::make_unique<WishItem>("Cheap", 10.0));
    manager.addItem(std::make_unique<WishItem>("Medium", 50.0));
    manager.addItem(std::make_unique<WishItem>("Expensive", 500.0));

    auto results = manager.findByPriceRange(20.0, 100.0);

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0]->getName(), "Medium");
}

// ==================== Statistics Tests ====================

TEST_F(WishlistManagerTest, GetTotalItemsEmpty) {
    WishlistManager manager("TestUser");

    EXPECT_EQ(manager.getTotalItems(), 0);
}

TEST_F(WishlistManagerTest, GetPurchasedCount) {
    WishlistManager manager("TestUser");

    auto item1 = std::make_unique<WishItem>("Item1", 10.0);
    auto item2 = std::make_unique<WishItem>("Item2", 20.0);
    auto item3 = std::make_unique<WishItem>("Item3", 30.0);

    item1->setPurchased(true);
    item3->setPurchased(true);

    manager.addItem(std::move(item1));
    manager.addItem(std::move(item2));
    manager.addItem(std::move(item3));

    EXPECT_EQ(manager.getPurchasedCount(), 2);
}

TEST_F(WishlistManagerTest, GetTotalValue) {
    WishlistManager manager("TestUser");

    manager.addItem(std::make_unique<WishItem>("Item1", 10.0));
    manager.addItem(std::make_unique<WishItem>("Item2", 20.0));
    manager.addItem(std::make_unique<WishItem>("Item3", 30.0));

    EXPECT_DOUBLE_EQ(manager.getTotalValue(), 60.0);
}

TEST_F(WishlistManagerTest, GetPurchasedValue) {
    WishlistManager manager("TestUser");

    auto item1 = std::make_unique<WishItem>("Item1", 10.0);
    auto item2 = std::make_unique<WishItem>("Item2", 20.0);
    auto item3 = std::make_unique<WishItem>("Item3", 30.0);

    item1->setPurchased(true);
    item3->setPurchased(true);

    manager.addItem(std::move(item1));
    manager.addItem(std::move(item2));
    manager.addItem(std::move(item3));

    EXPECT_DOUBLE_EQ(manager.getPurchasedValue(), 40.0);
}

TEST_F(WishlistManagerTest, GetRemainingValue) {
    WishlistManager manager("TestUser");

    auto item1 = std::make_unique<WishItem>("Item1", 10.0);
    auto item2 = std::make_unique<WishItem>("Item2", 20.0);

    item1->setPurchased(true);

    manager.addItem(std::move(item1));
    manager.addItem(std::move(item2));

    EXPECT_DOUBLE_EQ(manager.getRemainingValue(), 20.0);
}

// ==================== Sort Tests ====================

TEST_F(WishlistManagerTest, SortByPrice) {
    WishlistManager manager("TestUser");

    manager.addItem(std::make_unique<WishItem>("Expensive", 100.0));
    manager.addItem(std::make_unique<WishItem>("Cheap", 10.0));
    manager.addItem(std::make_unique<WishItem>("Medium", 50.0));

    manager.sort(SortOrder::BY_PRICE_ASC);

    const auto& items = manager.getItems();
    EXPECT_EQ(items[0]->getName(), "Cheap");
    EXPECT_EQ(items[1]->getName(), "Medium");
    EXPECT_EQ(items[2]->getName(), "Expensive");
}

TEST_F(WishlistManagerTest, SortByName) {
    WishlistManager manager("TestUser");

    manager.addItem(std::make_unique<WishItem>("Zebra", 10.0));
    manager.addItem(std::make_unique<WishItem>("Apple", 20.0));
    manager.addItem(std::make_unique<WishItem>("Mango", 30.0));

    manager.sort(SortOrder::BY_NAME);

    const auto& items = manager.getItems();
    EXPECT_EQ(items[0]->getName(), "Apple");
    EXPECT_EQ(items[1]->getName(), "Mango");
    EXPECT_EQ(items[2]->getName(), "Zebra");
}

// ==================== Bulk Operations ====================

TEST_F(WishlistManagerTest, MarkAllPurchased) {
    WishlistManager manager("TestUser");

    manager.addItem(std::make_unique<WishItem>("Item1", 10.0));
    manager.addItem(std::make_unique<WishItem>("Item2", 20.0));
    manager.addItem(std::make_unique<WishItem>("Item3", 30.0));

    manager.markAllPurchased();

    EXPECT_EQ(manager.getPurchasedCount(), 3);
}

TEST_F(WishlistManagerTest, ClearPurchased) {
    WishlistManager manager("TestUser");

    auto item1 = std::make_unique<WishItem>("Item1", 10.0);
    auto item2 = std::make_unique<WishItem>("Item2", 20.0);

    item1->setPurchased(true);

    manager.addItem(std::move(item1));
    manager.addItem(std::move(item2));

    manager.clearAllPurchased();

    EXPECT_EQ(manager.getTotalItems(), 1);
    EXPECT_EQ(manager.getItems()[0]->getName(), "Item2");
}