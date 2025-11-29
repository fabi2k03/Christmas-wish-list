//
// Created by Fabian Kopf on 24.11.25.
//

#ifndef CHISTMAS_WISHLIST_DATABASE_HANDLER_H
#define CHISTMAS_WISHLIST_DATABASE_HANDLER_H

#include <string>
#include <vector>
#include <memory>
#include <sqlite3.h>
#include "../include/wishlist.h"
#include "../include/wishlist_manager.h"

class DatabaseHandler {
private:
    sqlite3 *db;
    std::string dbPath;

    bool executeSQL(const std::string &sql);

    bool prepareStatement(const std::string &sql, sqlite3_stmt **stmt);

public:
    DatabaseHandler(const std::string &dbPath = "wishlist.db");

    ~DatabaseHandler();

    //Database initialization
    bool initialize();

    bool createTables();

    //User operations
    bool createUser(const std::string &username);

    int getUserId(const std::string &username);

    bool userExists(const std::string &username);

    std::vector<std::string> getAllUsers();

    // Item operations
    bool saveItem(const WishItem &item, const std::string &owner);

    bool updateItem(const WishItem &item, const std::string &owner);

    bool deleteItem(int itemId, const std::string &owner);

    std::vector<std::unique_ptr<WishItem> > loadItems(const std::string &owner);

    //Budget operations
    bool saveBudget(const Budget &budget, const std::string &owner);

    Budget loadBudget(const std::string &owner);

    //Statistics
    int getTotalItemsCount(const std::string &owner);

    double getTotalValue(const std::string &owner);

    //Utility
    bool clearAllData(const std::string &owner);

    bool vacuum(); //Optimize database
    std::string getLastError() const;

    int getGlobalMaxItemId();
};

#endif //CHISTMAS_WISHLIST_DATABASE_HANDLER_H
