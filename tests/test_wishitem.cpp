//
// Created by Fabian Kopf on 19.11.25.
//

#include <gtest/gtest.h>
#include "../include/wishlist.h"
#include "../include/logger.h"

class WishItemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Disable logging during tests
        Logger::getInstance().setLogLevel(LogLevel::NONE);

        // Reset static ID counter (if needed for consistent tests)
        // Note: You might need to add a static reset method to WishItem
    }

    void TearDown() override {
        // Cleanup after each test
    }
};

// ==================== Constructor Tests ====================

TEST_F(WishItemTest, DefaultConstructor) {
    WishItem item;

    EXPECT_EQ(item.getId(), 0);
    EXPECT_EQ(item.getName(), "");
    EXPECT_EQ(item.getPrice(), 0.0);
    EXPECT_FALSE(item.isPurchased());
    EXPECT_EQ(item.getCategory(), Category::OTHER);
    EXPECT_EQ(item.getPriority(), Priority::MEDIUM);
    EXPECT_EQ(item.getNotes(), "");
    EXPECT_EQ(item.getLink(), "");
}

TEST_F(WishItemTest, ParameterizedConstructor) {
    WishItem item("PlayStation 5", 499.99, Category::ELECTRONICS);

    EXPECT_GT(item.getId(), 0);
    EXPECT_EQ(item.getName(), "PlayStation 5");
    EXPECT_EQ(item.getPrice(), 499.99);
    EXPECT_FALSE(item.isPurchased());
    EXPECT_EQ(item.getCategory(), Category::ELECTRONICS);
    EXPECT_EQ(item.getPriority(), Priority::MEDIUM);
}

TEST_F(WishItemTest, CopyConstructor) {
    WishItem original("Book", 29.99, Category::BOOKS);
    original.setPriority(Priority::HIGH);
    original.setNotes("Must read");

    WishItem copy(original);

    // IDs should be different (new ID assigned)
    EXPECT_EQ(copy.getId(), original.getId());

    // But data should be the same
    EXPECT_EQ(copy.getName(), original.getName());
    EXPECT_EQ(copy.getPrice(), original.getPrice());
    EXPECT_EQ(copy.getCategory(), original.getCategory());
    EXPECT_EQ(copy.getPriority(), original.getPriority());
    EXPECT_EQ(copy.getNotes(), original.getNotes());
}

TEST_F(WishItemTest, MoveConstructor) {
    WishItem original("Laptop", 1299.99);
    std::string originalName = original.getName();

    WishItem moved(std::move(original));

    EXPECT_EQ(moved.getName(), originalName);
    EXPECT_EQ(moved.getPrice(), 1299.99);
}

// ==================== Setter Tests ====================

TEST_F(WishItemTest, SetName) {
    WishItem item;
    item.setName("New Name");
    EXPECT_EQ(item.getName(), "New Name");
}

TEST_F(WishItemTest, SetPrice) {
    WishItem item;
    item.setPrice(99.99);
    EXPECT_DOUBLE_EQ(item.getPrice(), 99.99);
}

TEST_F(WishItemTest, SetPriceNegative) {
    WishItem item;
    item.setPrice(-10.0);
    EXPECT_DOUBLE_EQ(item.getPrice(), -10.0);
    // Note: You might want to add validation to prevent negative prices
}

TEST_F(WishItemTest, SetPurchased) {
    WishItem item;
    EXPECT_FALSE(item.isPurchased());

    item.setPurchased(true);
    EXPECT_TRUE(item.isPurchased());

    item.setPurchased(false);
    EXPECT_FALSE(item.isPurchased());
}

TEST_F(WishItemTest, SetCategory) {
    WishItem item;
    item.setCategory(Category::TOYS);
    EXPECT_EQ(item.getCategory(), Category::TOYS);

    item.setCategory(Category::SPORTS);
    EXPECT_EQ(item.getCategory(), Category::SPORTS);
}

TEST_F(WishItemTest, SetPriority) {
    WishItem item;
    item.setPriority(Priority::URGENT);
    EXPECT_EQ(item.getPriority(), Priority::URGENT);

    item.setPriority(Priority::LOW);
    EXPECT_EQ(item.getPriority(), Priority::LOW);
}

TEST_F(WishItemTest, SetNotes) {
    WishItem item;
    item.setNotes("Important notes here");
    EXPECT_EQ(item.getNotes(), "Important notes here");
}

TEST_F(WishItemTest, SetLink) {
    WishItem item;
    item.setLink("https://example.com/product");
    EXPECT_EQ(item.getLink(), "https://example.com/product");
}

// ==================== Operator Tests ====================

TEST_F(WishItemTest, ComparisonOperatorByPriority) {
    WishItem urgent("Item1", 10.0);
    urgent.setPriority(Priority::URGENT);

    WishItem low("Item2", 100.0);
    low.setPriority(Priority::LOW);

    // Urgent should be "less than" (higher priority) than Low
    EXPECT_TRUE(urgent < low);
    EXPECT_FALSE(low < urgent);
}

TEST_F(WishItemTest, ComparisonOperatorByPrice) {
    WishItem expensive("Item1", 500.0);
    expensive.setPriority(Priority::MEDIUM);

    WishItem cheap("Item2", 50.0);
    cheap.setPriority(Priority::MEDIUM);

    // With same priority, more expensive is "less than"
    EXPECT_TRUE(expensive < cheap);
    EXPECT_FALSE(cheap < expensive);
}

// ==================== Serialization Tests ====================

TEST_F(WishItemTest, Serialize) {
    WishItem item("Test Item", 123.45, Category::ELECTRONICS);
    item.setPriority(Priority::HIGH);
    item.setNotes("Test notes");
    item.setLink("https://test.com");
    item.setPurchased(true);

    std::string serialized = item.serialize();

    // Should contain all fields separated by |
    EXPECT_NE(serialized.find("Test Item"), std::string::npos);
    EXPECT_NE(serialized.find("123.45"), std::string::npos);
    EXPECT_NE(serialized.find("1"), std::string::npos); // purchased = true
    EXPECT_NE(serialized.find("Test notes"), std::string::npos);
    EXPECT_NE(serialized.find("https://test.com"), std::string::npos);
}

TEST_F(WishItemTest, DeserializeValid) {
    std::string data = "42|Test Product|99.99|1|2|3|Some notes|http://example.com";

    auto item = WishItem::deserialize(data);

    ASSERT_NE(item, nullptr);
    EXPECT_EQ(item->getId(), 42);
    EXPECT_EQ(item->getName(), "Test Product");
    EXPECT_DOUBLE_EQ(item->getPrice(), 99.99);
    EXPECT_TRUE(item->isPurchased());
    EXPECT_EQ(item->getCategory(), Category::ELECTRONICS);
    EXPECT_EQ(item->getPriority(), Priority::URGENT);
    EXPECT_EQ(item->getNotes(), "Some notes");
    EXPECT_EQ(item->getLink(), "http://example.com");
}

TEST_F(WishItemTest, DeserializeInvalidTooFewFields) {
    std::string data = "1|Name|10.0";

    auto item = WishItem::deserialize(data);

    EXPECT_EQ(item, nullptr);
}

TEST_F(WishItemTest, DeserializeEmptyString) {
    std::string data = "";

    auto item = WishItem::deserialize(data);

    EXPECT_EQ(item, nullptr);
}

TEST_F(WishItemTest, SerializeDeserializeRoundTrip) {
    WishItem original("Round Trip Item", 77.77, Category::BOOKS);
    original.setPriority(Priority::LOW);
    original.setNotes("Test round trip");
    original.setPurchased(true);

    std::string serialized = original.serialize();
    auto deserialized = WishItem::deserialize(serialized);

    ASSERT_NE(deserialized, nullptr);
    EXPECT_EQ(deserialized->getName(), original.getName());
    EXPECT_DOUBLE_EQ(deserialized->getPrice(), original.getPrice());
    EXPECT_EQ(deserialized->isPurchased(), original.isPurchased());
    EXPECT_EQ(deserialized->getCategory(), original.getCategory());
    EXPECT_EQ(deserialized->getPriority(), original.getPriority());
    EXPECT_EQ(deserialized->getNotes(), original.getNotes());
}

// ==================== String Conversion Tests ====================

TEST_F(WishItemTest, CategoryToString) {
    EXPECT_EQ(WishItem::categoryToString(Category::TOYS), "Toys");
    EXPECT_EQ(WishItem::categoryToString(Category::BOOKS), "Books");
    EXPECT_EQ(WishItem::categoryToString(Category::ELECTRONICS), "Electronics");
    EXPECT_EQ(WishItem::categoryToString(Category::CLOTHING), "Clothing");
    EXPECT_EQ(WishItem::categoryToString(Category::SPORTS), "Sports");
    EXPECT_EQ(WishItem::categoryToString(Category::OTHER), "Other");
}

TEST_F(WishItemTest, StringToCategory) {
    EXPECT_EQ(WishItem::stringToCategory("toys"), Category::TOYS);
    EXPECT_EQ(WishItem::stringToCategory("TOYS"), Category::TOYS);
    EXPECT_EQ(WishItem::stringToCategory("books"), Category::BOOKS);
    EXPECT_EQ(WishItem::stringToCategory("electronics"), Category::ELECTRONICS);
    EXPECT_EQ(WishItem::stringToCategory("clothing"), Category::CLOTHING);
    EXPECT_EQ(WishItem::stringToCategory("sports"), Category::SPORTS);
    EXPECT_EQ(WishItem::stringToCategory("other"), Category::OTHER);
    EXPECT_EQ(WishItem::stringToCategory("invalid"), Category::OTHER);
}

TEST_F(WishItemTest, PriorityToString) {
    EXPECT_EQ(WishItem::priorityToString(Priority::LOW), "Low");
    EXPECT_EQ(WishItem::priorityToString(Priority::MEDIUM), "Medium");
    EXPECT_EQ(WishItem::priorityToString(Priority::HIGH), "High");
    EXPECT_EQ(WishItem::priorityToString(Priority::URGENT), "Urgent");
}

TEST_F(WishItemTest, StringToPriority) {
    EXPECT_EQ(WishItem::stringToPriority("low"), Priority::LOW);
    EXPECT_EQ(WishItem::stringToPriority("LOW"), Priority::LOW);
    EXPECT_EQ(WishItem::stringToPriority("medium"), Priority::MEDIUM);
    EXPECT_EQ(WishItem::stringToPriority("high"), Priority::HIGH);
    EXPECT_EQ(WishItem::stringToPriority("urgent"), Priority::URGENT);
    EXPECT_EQ(WishItem::stringToPriority("invalid"), Priority::MEDIUM);
}

// ==================== Edge Cases ====================

TEST_F(WishItemTest, EmptyName) {
    WishItem item("", 10.0);
    EXPECT_EQ(item.getName(), "");
}

TEST_F(WishItemTest, ZeroPrice) {
    WishItem item("Free Item", 0.0);
    EXPECT_DOUBLE_EQ(item.getPrice(), 0.0);
}

TEST_F(WishItemTest, VeryLongName) {
    std::string longName(1000, 'A');
    WishItem item(longName, 10.0);
    EXPECT_EQ(item.getName(), longName);
}

TEST_F(WishItemTest, SpecialCharactersInName) {
    WishItem item("Item with | pipe & special <> chars", 10.0);
    EXPECT_EQ(item.getName(), "Item with | pipe & special <> chars");
}