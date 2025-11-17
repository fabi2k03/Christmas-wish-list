#include <iostream>
#include <valarray>

#include "utils.h"
#include "wishlist_manager.h"
#include "file_handler.h"

void displayMenu() {
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║   🎄 CHRISTMAS WISHLIST MANAGER 🎄      ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    std::cout << "1.    Add Item\n";
    std::cout << "2.    Display all Items\n";
    std::cout << "3.    Display Pending Items\n";
    std::cout << "4.    Display Purchased Items\n";
    std::cout << "5.    Display by Category\n";
    std::cout << "6.    Search by Name\n";
    std::cout << "7.    Mark as Purchased\n";
    std::cout << "8.    Remove Item\n";
    std::cout << "9.    View Statistics\n";
    std::cout << "10.   Sort Items\n";
    std::cout << "11.   Save Wishlist\n";
    std::cout << "12.   Load Wishlist\n";
    std::cout << "13.   Export to CSV\n";
    std::cout << "0.    Exit\n";
}

void addItemInteractive(WishlistManager &manager) {
    auto item = std::make_unique<WishItem>();

    std::string name = Utils::getStringInput("Item name: ");
    item->setName(name);

    double price = Utils::getDoubleInput("Price (€): ");
    item->setPrice(price);

    std::cout << "\nCategories: toys, books, electronics, clothing, sports, other\n";
    std::string catStr = Utils::getStringInput("Category: ");
    item->setCategory(WishItem::stringToCategory(catStr));

    std::cout << "\nPriorities: low, medium, high, urgent\n";
    std::string prioStr = Utils::getStringInput("Priority: ");
    item->setPriority(WishItem::stringToPriority(prioStr));

    std::string notes = Utils::getStringInput("Notes (optional): ");
    if (!notes.empty()) item->setNotes(notes);

    std::string link = Utils::getStringInput("Link (optional): ");
    if (!link.empty()) item->setLink(link);

    manager.addItem(std::move(item));
    std::cout << "\n Item added successfully!\n";
}

void searchByName(WishlistManager &manager) {
    std::string query = Utils::getStringInput("Search for: ");
    auto results = manager.findByName(query);

    if (results.empty()) {
        std::cout << "No items found matching '" << query;
    }
    std::cout << "\n=== SEARCH RESULTS ===\n";
    std::cout << "Found " << results.size() << " item(s):\n\n";
    for (const auto *item: results) {
        std::cout << *item << "\n";
        std::cout << "  Category: " << WishItem::categoryToString(item->getCategory()) << "\n";
        std::cout << "  Priority: " << WishItem::priorityToString(item->getPriority()) << "\n";
        if (!item->getNotes().empty()) {
            std::cout << "  Notes: " << item->getNotes() << "\n";
        }
        std::cout << "\n";
    }
}

void markAsPurchased(WishlistManager &manager) {
    manager.displayPending();

    if (manager.getTotalItems() == 0) return;

    int id = Utils::getIntInput("\nEnter item ID to mark as purchased: ", 1);
    WishItem *item = manager.findById(id);

    if (item) {
        item->setPurchased(true);
        std::cout << "✓ Item '" << item->getName() << "' marked as purchased!\n";
    } else {
        std::cout << "Error: Item with ID " << id << " not found!\n";
    }
}

void removeItem(WishlistManager &manager) {
    manager.displayAll();

    if (manager.getTotalItems() == 0) return;

    int id = Utils::getIntInput("\nEnter item ID to remove: ", 1);

    if (Utils::confirm("Are you sure you want to remove this item?")) {
        if (manager.removeItem(id)) {
            std::cout << "✓ Item removed successfully!\n";
        } else {
            std::cout << "Error: Item with ID " << id << " not found!\n";
        }
    }
}

void sortMenu(WishlistManager &manager) {
    std::cout << "\n=== SORT OPTIONS ===\n";
    std::cout << "1. By Priority\n";
    std::cout << "2. By Price (Ascending)\n";
    std::cout << "3. By Price (Descending)\n";
    std::cout << "4. By Name\n";
    std::cout << "5. By Category\n";
    std::cout << "6. By ID\n";

    int choice = Utils::getIntInput("Choose sort order: ", 1, 6);

    switch (choice) {
        case 1: manager.sort(SortOrder::BY_PRIORITY);
            break;
        case 2: manager.sort(SortOrder::BY_PRICE_ASC);
            break;
        case 3: manager.sort(SortOrder::BY_PRICE_DESC);
            break;
        case 4: manager.sort(SortOrder::BY_NAME);
            break;
        case 5: manager.sort(SortOrder::BY_CATEGORY);
            break;
        case 6: manager.sort(SortOrder::BY_ID);
            break;
    }

    std::cout << "✓ Items sorted!\n";
    manager.displayAll();
}

void saveWishlist(WishlistManager &manager, FileHandler &fileHandler) {
    if (fileHandler.save(manager)) {
        std::cout << "✓ Wishlist saved successfully!\n";
    } else {
        std::cout << "✗ Failed to save wishlist!\n";
    }
}

void loadWishlist(WishlistManager &manager, FileHandler &fileHandler) {
    if (Utils::confirm("This will replace current wishlist. Continue?")) {
        if (fileHandler.load(manager)) {
            std::cout << "✓ Wishlist loaded successfully!\n";
            manager.displayAll();
        } else {
            std::cout << "✗ Failed to load wishlist!\n";
        }
    }
}

void exportToCSV(WishlistManager &manager, FileHandler &fileHandler) {
    std::string filename = Utils::getStringInput("Enter CSV filename (e.g., wishlist.csv): ");

    if (fileHandler.exportToCSV(manager, filename)) {
        std::cout << "✓ Exported to " << filename << " successfully!\n";
    } else {
        std::cout << "✗ Failed to export to CSV!\n";
    }
}

void importFromCSV(WishlistManager &manager, FileHandler &fileHandler) {
    std::string filename = Utils::getStringInput("Enter CSV filename to import: ");

    if (Utils::confirm("This will ADD items to current wishlist. Continue?")) {
        if (fileHandler.importFromCSV(manager, filename)) {
            std::cout << "✓ Imported from " << filename << " successfully!\n";
            manager.displayAll();
        } else {
            std::cout << "✗ Failed to import from CSV!\n";
        }
    }
}


int main() {
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║  Welcome to Christmas Wishlist App  ║\n";
    std::cout << "╚══════════════════════════════════════╝\n\n";

    std::string ownerName = Utils::getStringInput("Enter your name: ");
    std::string filename = "wishlist_"+ownerName+".dat";
    std::cout << "\nYour wishlist will be saved to " << filename << std::endl;

    WishlistManager manager(ownerName);
    FileHandler fileHandler(filename);

    std::cout << "\nChecking for existing wishlist...\n";
    bool loaded = fileHandler.load(manager);

    if (loaded) {
        std::cout << "Welcome back, " << ownerName << "!" << std::endl;
        std::cout << "  Found " << manager.getTotalItems() << " item(s) in your wishlist.\n";
    }else {
        std::cout << "Starting fresh wishlist for " << ownerName << "!\n";
    }

    int choice;
    bool running = true;

    while (running) {
        displayMenu();
        choice = Utils::getIntInput("\nYour choice: ", 0, 14);

        switch (choice) {
            case 1:
                addItemInteractive(manager);
                Utils::pause();
                break;

            case 2:
                manager.displayAll();
                Utils::pause();
                break;

            case 3:
                manager.displayPending();
                Utils::pause();
                break;

            case 4:
                manager.displayPurchased();
                Utils::pause();
                break;

            case 5:
                manager.displayByCategory();
                Utils::pause();
                break;

            case 6:
                searchByName(manager);
                Utils::pause();
                break;

            case 7:
                markAsPurchased(manager);
                Utils::pause();
                break;

            case 8:
                removeItem(manager);
                Utils::pause();
                break;

            case 9:
                manager.displayStatistics();
                Utils::pause();
                break;

            case 10:
                sortMenu(manager);
                Utils::pause();
                break;

            case 11:
                saveWishlist(manager, fileHandler);
                Utils::pause();
                break;

            case 12:
                loadWishlist(manager, fileHandler);
                Utils::pause();
                break;

            case 13:
                exportToCSV(manager, fileHandler);
                Utils::pause();
                break;

            case 14:
                importFromCSV(manager, fileHandler);
                Utils::pause();
                break;

            case 0:
                if (Utils::confirm("Save before exiting?")) {
                    fileHandler.save(manager);
                }
                std::cout << "\n🎄 Merry Christmas! Goodbye! 🎅\n";
                running = false;
                break;

            default:
                std::cout << "Invalid choice!\n";
                Utils::pause();
        }
    }

    return 0;
}
