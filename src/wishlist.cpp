//
// Created by Fabian Kopf on 11.11.25.
//
#include "../include/wishlist.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include "../include/logger.h"
#include <memory>
#include <vector>

int WishItem::nextId = 1;

WishItem::WishItem()
    : id(nextId++), name(""), price(0.0), purchased(false), category(Category::OTHER), priority(Priority::MEDIUM),
      notes(""), link("") {
    LOG_DEBUG("WishItem: Default constructor called (ID: ", id, ")");
}

WishItem::WishItem(const std::string &name, double price, Category cat)
    : id(nextId++), name(name), price(price), purchased(false), category(cat), priority(Priority::MEDIUM), notes(""),
      link("") {
    LOG_DEBUG("[WishItem] Parameterized constructor called (ID: ", id, ", Name: ", name , ")");
}

WishItem::~WishItem() {
   LOG_DEBUG("[WishItem] Destructor called (ID: " , id , ", Name: " , name , ")");
}

WishItem::WishItem(const WishItem &other)
    : id(nextId++), name(other.name), price(other.price), purchased(other.purchased), category(other.category),
      priority(other.priority), notes(other.notes), link(other.link) {
    LOG_DEBUG("[WishItem] Copy constructor called (New ID: " , id , ", From: " , other.id , ")");
}

WishItem &WishItem::operator=(const WishItem &other) {
    if (this != &other) {
        name = other.name;
        price = other.price;
        purchased = other.purchased;
        category = other.category;
        priority = other.priority;
        notes = other.notes;
        link = other.link;
        LOG_DEBUG("[WishItem] Copy assignment called (ID: " , id , ", Name: " , name , ")");
    }
    return *this;
}

WishItem::WishItem(WishItem &&other) noexcept
    : id(other.id), name(std::move(other.name)), price(other.price),
      purchased(other.purchased), category(other.category),
      priority(other.priority), notes(std::move(other.notes)),
      link(std::move(other.link)) {
    other.id = 0;
    other.price = 0.0;
    LOG_DEBUG("[WishItem] Move constructor called (ID: " , id , ")");
}

WishItem &WishItem::operator=(WishItem &&other) noexcept {
    if (this != &other) {
        id = other.id;
        name = std::move(other.name);
        price = other.price;
        purchased = other.purchased;
        category = other.category;
        priority = other.priority;
        notes = std::move(other.notes);
        link = std::move(other.link);
        other.id = 0;
        other.price = 0.0;
        LOG_DEBUG("[WishItem] Move assignment called");
    }
    return *this;
}

void WishItem::setName(const std::string &name) { this->name = name; }
void WishItem::setPrice(double price) { this->price = price; }
void WishItem::setPurchased(bool purchased) { this->purchased = purchased; }
void WishItem::setCategory(Category cat) { this->category = cat; }
void WishItem::setPriority(Priority priority) { this->priority = priority; }
void WishItem::setNotes(const std::string &notes) { this->notes = notes; }
void WishItem::setLink(const std::string &link) { this->link = link; }

bool WishItem::operator<(const WishItem &other) const {
    if (priority != other.priority) {
        return priority > other.priority;
    }
    return price > other.price;
}

bool WishItem::operator>(const WishItem &other) const {
    return other < *this;
}

std::ostream &operator<<(std::ostream &os, const WishItem &item) {
    os << "[" << item.id << "] " << item.name << " ($" << std::fixed << std::setprecision(2) << item.price << ")";
    return os;
}

std::string WishItem::toString() const {
    std::stringstream ss;
    ss << "ID: " << id << "\n"
            << "Name: " << name << "\n"
            << "Price: $" << std::fixed << std::setprecision(2) << price << "\n"
            << "Status: " << (purchased ? "Purchased" : "Pending") << "\n"
            << "Category: " << categoryToString(category) << "\n"
            << "Priority: " << priorityToString(priority) << "\n";
    if (!notes.empty()) ss << "Notes: " << notes << "\n";
    if (!link.empty()) ss << "Link: " << link << "\n";
    return ss.str();
}

std::string WishItem::serialize() const {
    std::stringstream ss;
    ss << id << "|" << name << "|" << price << "|" << purchased << "|" << static_cast<int>(category) << "|"
            << static_cast<int>(priority) << "|" << notes << "|" << link;
    return ss.str();
}

std::unique_ptr<WishItem> WishItem::deserialize(const std::string &data) {
    std::stringstream ss(data);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.size() < 6) {
        LOG_ERROR("[WishItem] deserialize: Not enough tokens (", tokens.size() , "), expected at least 6");
        return nullptr;
    }

    auto item = std::make_unique<WishItem>();

    try {
        item->id = std::stoi(tokens[0]);
        item->name = tokens[1];
        item->price = std::stod(tokens[2]);
        item->purchased = (tokens[3] == "1");
        item->category = static_cast<Category>(std::stoi(tokens[4]));
        item->priority = static_cast<Priority>(std::stoi(tokens[5]));

        if (tokens.size() > 6) {
            item->notes = tokens[6];
        }
        if (tokens.size() > 7) {
            item->link = tokens[7];
        }

        if (item->id >= nextId) {
            nextId = item->id + 1;
        }

       LOG_DEBUG("[WishItem] Deserialized: ID=" , item->id , ", Name=" , item->name);

    } catch (const std::exception& e) {
        LOG_ERROR("[WishItem] deserialize error: " , e.what());
        return nullptr;
    }

    return item;
}

std::string WishItem::categoryToString(Category cat) {
    switch (cat) {
        case Category::TOYS: return "Toys";
        case Category::BOOKS: return "Books";
        case Category::ELECTRONICS: return "Electronics";
        case Category::CLOTHING: return "Clothing";
        case Category::SPORTS: return "Sports";
        default: return "Other";
    }
}

std::string WishItem::priorityToString(Priority prio) {
    switch (prio) {
        case Priority::LOW: return "Low";
        case Priority::MEDIUM: return "Medium";
        case Priority::HIGH: return "High";
        case Priority::URGENT: return "Urgent";
        default: return "Medium";
    }
}

Category WishItem::stringToCategory(const std::string &str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "toys") {
        return Category::TOYS;
    }
    if (lower == "books") {
        return Category::BOOKS;
    }
    if (lower == "electronics") {
        return Category::ELECTRONICS;
    }
    if (lower == "sports") {
        return Category::SPORTS;
    }
    if (lower == "clothing") {
        return Category::CLOTHING;
    }
    return Category::OTHER;
}

Priority WishItem::stringToPriority(const std::string &str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "low") {
        return Priority::LOW;
    }
    if (lower == "medium") {
        return Priority::MEDIUM;
    }
    if (lower == "high") {
        return Priority::HIGH;
    }
    if (lower == "urgent") {
        return Priority::URGENT;
    }
    return Priority::MEDIUM;
}

