//
// Created by Fabian Kopf on 19.11.25.
//
#include <gtest/gtest.h>
#include "../include/file_handler.h"
#include "../include/wishlist_manager.h"
#include "../include/logger.h"
#include <fstream>
#include <filesystem>

class FileHandlerTest : public ::testing::Test {
protected:
    std::string testFile = "test_wishlist.dat";
    std::string testCsvFile = "test_wishlist.csv";

    void SetUp() override {
        Logger::getInstance().setLogLevel(LogLevel::NONE);

        // Clean up test files
        std::filesystem::remove(testFile);
        std::filesystem::remove(testCsvFile);
    }

    void TearDown() override {
        // Clean up test files
        std::filesystem::remove(testFile);
        std::filesystem::remove(testCsvFile);
    }
};

// ==================== Save/Load Tests ====================

TEST_F(FileHandlerTest, SaveEmptyWishlist) {
    WishlistManager manager("TestUser");
    FileHandler handler(testFile);

    EXPECT_TRUE(handler.save(manager));
    EXPECT_TRUE(std::filesystem::exists(testFile));
}

TEST_F(FileHandlerTest, SaveAndLoadWishlist) {
    // Save
    WishlistManager saveManager("TestUser");
    saveManager.addItem(std::make_unique<WishItem>("Item1", 10.0));
    saveManager.addItem(std::make_unique<WishItem>("Item2", 20.0));

    FileHandler handler(testFile);
    EXPECT_TRUE(handler.save(saveManager));

    // Load
    WishlistManager loadManager("TempUser");
    EXPECT_TRUE(handler.load(loadManager));

    EXPECT_EQ(loadManager.getOwner(), "TestUser");
    EXPECT_EQ(loadManager.getTotalItems(), 2);
}

TEST_F(FileHandlerTest, LoadNonExistentFile) {
    WishlistManager manager("TestUser");
    FileHandler handler("nonexistent.dat");

    EXPECT_FALSE(handler.load(manager));
}

/*TEST_F(FileHandlerTest, SaveAndLoadWithSpecialCharacters) {
    WishlistManager saveManager("User|With|Pipes");
    auto item = std::make_unique<WishItem>("Item | with | pipes", 99.99);
    item->setNotes("Notes | with | pipes");
    saveManager.addItem(std::move(item));

    FileHandler handler(testFile);
    handler.save(saveManager);

    WishlistManager loadManager("Temp");
    handler.load(loadManager);

    EXPECT_EQ(loadManager.getTotalItems(), 1);
}*/

// ==================== CSV Export/Import Tests ====================

TEST_F(FileHandlerTest, ExportToCSV) {
    WishlistManager manager("TestUser");
    manager.addItem(std::make_unique<WishItem>("Item1", 10.0, Category::TOYS));

    FileHandler handler(testFile);
    EXPECT_TRUE(handler.exportToCSV(manager, testCsvFile));
    EXPECT_TRUE(std::filesystem::exists(testCsvFile));
}

TEST_F(FileHandlerTest, ExportAndImportCSV) {
    // Export
    WishlistManager exportManager("TestUser");
    exportManager.addItem(std::make_unique<WishItem>("CSV Item", 45.0, Category::BOOKS));

    FileHandler handler(testFile);
    handler.exportToCSV(exportManager, testCsvFile);

    // Import
    WishlistManager importManager("TestUser");
    EXPECT_TRUE(handler.importFromCSV(importManager, testCsvFile));

    EXPECT_EQ(importManager.getTotalItems(), 1);
}

TEST_F(FileHandlerTest, ImportNonExistentCSV) {
    WishlistManager manager("TestUser");
    FileHandler handler(testFile);

    EXPECT_FALSE(handler.importFromCSV(manager, "nonexistent.csv"));
}