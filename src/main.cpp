#include <iostream>
#include <valarray>
#include <memory>

#include "../include/utils.h"
#include "../include/wishlist_manager.h"
#include "../include/file_handler.h"

#include "../include/logger.h"
#include "../include/database_handler.h"


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
    std::cout << "11.   Save Wishlist (Database)\n";
    std::cout << "12.   Load Wishlist (Database)\n";
    std::cout << "13.   Export to CSV\n";
    std::cout << "14.   Import from CSV\n";
    std::cout << "15.   Switch User\n";
    std::cout << "16.   Budget Management \n";
    std::cout << "0.    Exit\n";
}

void budgetMenu(WishlistManager &manager) {
    while (true) {
        std::cout << "\n╔══════════════════════════════════════╗\n";
        std::cout << "║       💰 BUDGET MANAGEMENT 💰        ║\n";
        std::cout << "╚══════════════════════════════════════╝\n";
        std::cout << "1. Set Budget\n";
        std::cout << "2. View Budget Status\n";
        std::cout << "3. Enable Budget Tracking\n";
        std::cout << "4. Disable Budget Tracking\n";
        std::cout << "5. Reset Spent Amount\n";
        std::cout << "6. Sync with Purchases\n";
        std::cout << "0. Back to Main Menu\n";

        int choice = Utils::getIntInput("\nYour choice: ", 0, 6);

        switch (choice) {
            case 1: {
                double amount = Utils::getDoubleInput("Enter maximum budget (€): ", 0.0);
                manager.setBudget(amount);
                std::cout << "Budget set to € " << std::fixed << std::setprecision(2) << amount << "\n";
                manager.displayBudgetStatus();
                Utils::pause();
                break;
            }
            case 2:
                manager.displayBudgetStatus();
                Utils::pause();
                break;
            case 3:
                manager.enableBudget();
                std::cout << "Budget tracking enabled!\n";
                manager.displayBudgetStatus();
                Utils::pause();
                break;
            case 4:
                manager.disableBudget();
                std::cout << "Budget tracking disabled!\n";
                Utils::pause();
                break;
            case 5:
                if (Utils::confirm("Reset spent amount to 0€? This will clear purchase tracking.")) {
                    manager.resetBudget();
                    std::cout << "Budget spent amount reset to 0€\n";
                }
                Utils::pause();
                break;
            case 6:
                manager.syncBudgetWithPurchases();
                std::cout << "Budget synced with purchased item \n";
                manager.displayBudgetStatus();
                Utils::pause();
                break;
            case 0:
                return;
            default:
                std::cout << "Invalid choice!\n";
                Utils::pause();
        }
    }
}

void addItemInteractive(WishlistManager &manager) {
    auto item = std::make_unique<WishItem>();

    std::string name = Utils::getStringInput("Item name: ");
    item->setName(name);

    double price = Utils::getDoubleInput("Price (€): ");
    item->setPrice(price);

    if (!manager.checkBudgetBevorAdd(price)) {
        if (!Utils::confirm("Add item anyway")) {
            std::cout << "Item not added!\n";
            return;
        }
    }

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
        std::cout << "No items found matching " << query;
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
        manager.syncBudgetWithPurchases();
        std::cout << "✓ Item '" << item->getName() << "' marked as purchased!\n";
    } else {
        std::cout << "Error: Item with ID " << id << " not found!\n";
    }
    if (manager.getDatabaseHandler())
        manager.getDatabaseHandler()->updateItem(*item, manager.getOwner());
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

    std::cout << "Items sorted!\n";
    manager.displayAll();
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

void switchUser(WishlistManager * &manager, FileHandler * &fileHandler, std::string &currentOwner, DatabaseHandler *dbHandler) {
    std::cout << "=== SWITCH USER ===" << std::endl;

    if (manager) {
        if (!Utils::confirm("Save current wishlist before switching??")) {
            if (manager->saveToDatabase()){
                std::cout << "Saved wishlist for "<< currentOwner << " to DB." << std::endl;
                LOG_INFO("Wishlist saved to database for user: ", currentOwner);
            } else {
                std::cout << "Failed to save wishlist for " << currentOwner << " to DB." << std::endl;
                LOG_ERROR("Failed to save wishlist for user: ", currentOwner);
            }
        }
    }

    std::string newOwner = Utils::getStringInput("Enter new username: ");

    if (newOwner.empty()) {
        std::cout << "Invalid username. Switch cancelled.\n";
        return;
    }

    if (newOwner == currentOwner) {
        std::cout << "Already logged in as " << newOwner << ". Switch cancelled.\n";
        return;
    }

    //Clean up old manager and file handler
    delete manager;
    delete fileHandler;

    currentOwner = newOwner;

    manager = new WishlistManager(newOwner);
    manager->setDatabaseHandler(dbHandler);

    //Ensure user exists in database
    dbHandler->createUser(newOwner);

    bool loaded = manager->loadFromDatabase();

    if (loaded) {
        std::cout << "Loaded wishlist for " << newOwner << "\n";
        std::cout << "Found " << manager->getTotalItems() << " item(s).\n";
    } else {
        std::cout << "Starting fresh wishlist for " << newOwner << "\n";
    }

    std::string filename = "wishlist_" + newOwner + ".dat";
    fileHandler = new FileHandler(filename);
}

int main() {
    //Config Logger
    Logger &logger = Logger::getInstance();
    logger.setLogLevel(LogLevel::DEBUG);
    logger.enableConsoleOutput(false);
    logger.setLogFile("wishlist_app.log");

    LOG_INFO("========================================");
    LOG_INFO("Application started");
    LOG_INFO("========================================");

    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║  Welcome to Christmas Wishlist App  ║\n";
    std::cout << "╚══════════════════════════════════════╝\n\n";

    //Initialize database
    DatabaseHandler dbHandler("wishlist.db");
    if (!dbHandler.initialize()) {
        std::cerr << "Failed to initialize database!\n";
        return 1;
    }

    std::string ownerName = Utils::getStringInput("Enter your name: ");
    std::string filename = "wishlist_" + ownerName + ".dat";
    std::cout << "\nYour wishlist will be saved to database and backed up to " << filename <<
            " (if file operations used).\n"; // Text angepasst

    WishlistManager *manager = new WishlistManager(ownerName);
    manager->setDatabaseHandler(&dbHandler); // DB-Handler setzen

    std::cout << "\nChecking for existing wishlist in Database...\n";
    bool loaded = manager->loadFromDatabase(); // Lade Items und Budget aus DB

    if (loaded) {
        std::cout << "Welcome back, " << ownerName << "!" << std::endl;
        std::cout << "  Found " << manager->getTotalItems() << " item(s) in your wishlist.\n";
    } else {
        std::cout << "Starting fresh wishlist for " << ownerName << "!\n";
    }

    FileHandler *fileHandler = new FileHandler(filename);


    int choice;
    bool running = true;

    while (running) {
        displayMenu();
        choice = Utils::getIntInput("\nYour choice: ", 0, 16);

        switch (choice) {
            case 1:
                addItemInteractive(*manager);
                Utils::pause();
                break;

            case 2:
                manager->displayAll();
                Utils::pause();
                break;

            case 3:
                manager->displayPending();
                Utils::pause();
                break;

            case 4:
                manager->displayPurchased();
                Utils::pause();
                break;

            case 5:
                manager->displayByCategory();
                Utils::pause();
                break;

            case 6:
                searchByName(*manager);
                Utils::pause();
                break;

            case 7:
                markAsPurchased(*manager); // Update in DB erfolgt intern
                Utils::pause();
                break;

            case 8:
                removeItem(*manager); // Remove in DB erfolgt intern
                Utils::pause();
                break;

            case 9:
                manager->displayStatistics();
                Utils::pause();
                break;

            case 10:
                sortMenu(*manager);
                Utils::pause();
                break;

            case 11:
                if (manager->saveToDatabase()) {
                    std::cout << "✓ Saved to database!\n";
                } else {
                    std::cout << "✗ Failed to save database!\n";
                }
                Utils::pause();
                break;

            case 12:
                if (manager->loadFromDatabase()) {
                    std::cout << "✓ Loaded from database!\n";
                } else {
                    std::cout << "✗ Could not load from database!\n";
                }
                Utils::pause();
                break;

            case 13:
                exportToCSV(*manager, *fileHandler);
                Utils::pause();
                break;

            case 14:
                importFromCSV(*manager, *fileHandler);
                Utils::pause();
                break;
            case 15:
                switchUser(manager, fileHandler, ownerName, &dbHandler);
                manager->setDatabaseHandler(&dbHandler); // DB-Handler für den NEUEN Manager setzen!
                Utils::pause();
                break;
            case 16:
                budgetMenu(*manager);
                break;

            case 0:
                if (Utils::confirm("Save to database before exiting?")) {
                    manager->saveToDatabase();
                }
                std::cout << "\n🎄 Merry Christmas! Goodbye! 🎅\n";
                running = false;
                break;

            default:
                std::cout << "Invalid choice!\n";
                Utils::pause();
        }
    }

    delete manager;
    delete fileHandler;

    LOG_INFO("========================================");
    LOG_INFO("Application exiting normally");
    LOG_INFO("========================================");

    return 0;
}
