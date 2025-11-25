//
// Created by Fabian Kopf on 10.11.25.
//

#ifndef CHISTMAS_WISHLIST_WISHLIST_H
#define CHISTMAS_WISHLIST_WISHLIST_H

#include <string>
#include <memory>

enum class Priority {
    LOW,
    MEDIUM,
    HIGH,
    URGENT
};

enum class Category {
    TOYS,
    BOOKS,
    ELECTRONICS,
    CLOTHING,
    SPORTS,
    OTHER
};

class WishItem {
private:
    static int nextId;
    int id;
    std::string name;
    double price;
    bool purchased;
    Category category;
    Priority priority;
    std::string notes;
    std::string link;

public:
    // Constructors
    WishItem();
    WishItem(const std::string& name, double price, Category cat = Category::OTHER);

    // Destructor
    ~WishItem();

    // Copy constructor
    WishItem(const WishItem& other);

    // Copy assignment
    WishItem& operator=(const WishItem& other);

    // Move constructor
    WishItem(WishItem&& other) noexcept;

    // Move assignment
    WishItem& operator=(WishItem&& other) noexcept;

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    double getPrice() const { return price; }
    bool isPurchased() const { return purchased; }
    Category getCategory() const { return category; }
    Priority getPriority() const { return priority; }
    std::string getNotes() const { return notes; }
    std::string getLink() const { return link; }

    // Setters
    void setName(const std::string& name);
    void setPrice(double price);
    void setPurchased(bool purchased);
    void setCategory(Category cat);
    void setPriority(Priority prio);
    void setNotes(const std::string& notes);
    void setLink(const std::string& link);
    void setId(int newId) { id = newId; }

    // Operators
    bool operator<(const WishItem& other) const;
    bool operator>(const WishItem& other) const;
    friend std::ostream& operator<<(std::ostream& os, const WishItem& item);

    // Utility
    std::string toString() const;
    std::string serialize() const;
    static std::unique_ptr<WishItem> deserialize(const std::string& data);

    // Static helper
    static std::string categoryToString(Category cat);
    static std::string priorityToString(Priority prio);
    static Category stringToCategory(const std::string& str);
    static Priority stringToPriority(const std::string& str);
};

#endif //CHISTMAS_WISHLIST_WISHLIST_H