//
// Created by Fabian Kopf on 24.11.25.
//

#include "include/database_handler.h"
#include "include/logger.h"
#include <iostream>
#include <sstream>

DatabaseHandler::DatabaseHandler(const std::string &dbPath) : db(nullptr), dbPath(dbPath) {
    LOG_INFO("DatabaseHandler: Created with path: ", dbPath);
}

DatabaseHandler::~DatabaseHandler() {
    if (db) {
        sqlite3_close(db);
        LOG_INFO("DatabaseHandler: Database connection closed");
    }
}

bool DatabaseHandler::initialize() {
    int result = sqlite3_open(dbPath.c_str(), &db);

    if (result != SQLITE_OK) {
        LOG_ERROR("DatabaseHandler: Failed to open database: ", sqlite3_errmsg(db));
        return false;
    }
    LOG_INFO("DatabaseHandler: Database opened successfully");
    executeSQL("PRAGMA foreign_keys = ON;");
    return createTables();
}

bool DatabaseHandler::createTables() {
    const char *createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
    )";

    const char *createItemsTable = R"(
        CREATE TABLE IF NOT EXISTS items(
            id INTEGER PRIMARY KEY,
            user_id INTEGER NOT NULL,
            name TEXT NOT NULL,
            price REAL NOT NULL,
            purchased INTEGER DEFAULT 0,
            category INTEGER DEFAULT 0,
            priority INTEGER DEFAULT 1,
            notes TEXT,
            link TEXT,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
        );
    )";

    const char *createBudgetTable = R"(
        CREATE TABLE IF NOT EXISTS budgets(
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER UNIQUE NOT NULL,
            max_budget REAL DEFAULT 0.0,
            spent_amount REAL DEFAULT 0.0,
            enabled INTEGER DEFAULT 0,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
        );
    )";

    const char *createPriceHistoryTable = R"(
        CREATE TABLE IF NOT EXISTS price_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            item_id INTEGER NOT NULL,
            price REAL NOT NULL,
            recorded_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(item_id) REFERENCES items(id) ON DELETE CASCADE
        );
    )";

    // Create indexes for better performance
    const char *createIndexes = R"(
        CREATE INDEX IF NOT EXISTS idx_items_user_id ON items(user_id);
        CREATE INDEX IF NOT EXISTS idx_items_purchased ON items(purchased);
        CREATE INDEX IF NOT EXISTS idx_items_category ON items(category);
    )";

    bool success = executeSQL(createUsersTable) && executeSQL(createItemsTable) && executeSQL(createBudgetTable) &&
                   executeSQL(createPriceHistoryTable) && executeSQL(createIndexes);
    if (success) {
        LOG_INFO("DatabaseHandler: All tables created successfully");
    } else {
        LOG_ERROR("DatabaseHandler: Failed to create tables");
    }

    return success;
}

bool DatabaseHandler::executeSQL(const std::string &sql) {
    char *errorMsg = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg);
    if (result != SQLITE_OK) {
        LOG_ERROR("DatabaseHandler: SQL error: ", errorMsg);
        sqlite3_free(errorMsg);
        return false;
    }

    return true;
}

bool DatabaseHandler::prepareStatement(const std::string &sql, sqlite3_stmt **stmt) {
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, stmt, nullptr);
    if (result != SQLITE_OK) {
        LOG_ERROR("DatabaseHandler: Failed to prepare statement: ", sqlite3_errmsg(db));
        return false;
    }

    return true;
}

bool DatabaseHandler::createUser(const std::string &username) {
    if (userExists(username)) {
        LOG_DEBUG("DatabaseHandler: User already exists: ", username);
        return true;
    }

    const char *sql = "INSERT INTO users (username) VALUES (?);";
    sqlite3_stmt *stmt;

    if (!prepareStatement(sql, &stmt)) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_OK) {
        LOG_ERROR("DatabaseHandler: Failed to create user: ", sqlite3_errmsg(db));
        return false;
    }

    LOG_INFO("DatabaseHandler: User created: ", username);
    return true;
}

int DatabaseHandler::getUserId(const std::string &username) {
    const char *sql = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt *stmt;

    if (!prepareStatement(sql, &stmt)) {
        return -1;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    int userId = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return userId;
}

bool DatabaseHandler::userExists(const std::string &username) {
    return getUserId(username) != -1;
}

std::vector<std::string> DatabaseHandler::getAllUsers() {
    std::vector<std::string> users;
    const char *sql = "SELECT username FROM users ORDER BY username;";
    sqlite3_stmt *stmt;

    if (!prepareStatement(sql, &stmt)) {
        return users;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        users.push_back(username);
    }

    sqlite3_finalize(stmt);
    return users;
}

bool DatabaseHandler::saveItem(const WishItem &item, const std::string &owner) {
    //Ensure user exists
    if (!createUser(owner)) {
        return false;
    }

    int userId = getUserId(owner);
    if (userId == -1) {
        return false;
    }

    const char *sql =
            R"(INSERT OR REPLACE INTO items (id, user_id, name, price, purchased, category, priority, notes, link, updated_at)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, CURRENT_TIMESTAMP);
    )";

    sqlite3_stmt *stmt;
    if (!prepareStatement(sql, &stmt)) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, item.getId());
    sqlite3_bind_int(stmt, 2, userId);
    sqlite3_bind_text(stmt, 3, item.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, item.getPrice());
    sqlite3_bind_int(stmt, 5, item.isPurchased());
    sqlite3_bind_int(stmt, 6, static_cast<int>(item.getCategory()));
    sqlite3_bind_int(stmt, 7, static_cast<int>(item.getPriority()));
    sqlite3_bind_text(stmt, 8, item.getNotes().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, item.getLink().c_str(), -1, SQLITE_TRANSIENT);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        LOG_ERROR("DatabaseHandler: Failed to save item: ", sqlite3_errmsg(db));
        return false;
    }

    LOG_DEBUG("DatabaseHandler: Item saved - ID: ", item.getId(), " Name: ", item.getName());
    return true;
}

bool DatabaseHandler::updateItem(const WishItem &item, const std::string &owner) {
    return saveItem(item, owner); // Insert or Replace handle updates
}

bool DatabaseHandler::deleteItem(int itemId, const std::string &owner) {
    int userId = getUserId(owner);
    if (userId == -1) {
        return false;
    }

    const char *sql = "DELETE FROM items WHERE id = ? AND user_id = ?;";
    sqlite3_stmt *stmt;

    if (!prepareStatement(sql, &stmt)) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, itemId);
    sqlite3_bind_int(stmt, 2, userId);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        LOG_ERROR("DatabaseHandler: Failed to delete item: ", sqlite3_errmsg(db));
        return false;
    }

    LOG_INFO("DatabaseHandler: Item deleted - ID: ", itemId);
    return true;
}

std::vector<std::unique_ptr<WishItem> > DatabaseHandler::loadItems(const std::string &owner) {
    std::vector<std::unique_ptr<WishItem> > items;

    int userId = getUserId(owner);
    if (userId == -1) {
        LOG_ERROR("DatabaseHandler: User not found: ", owner);
        return items;
    }

    const char *sql = R"(
        SELECT id, name, price, purchased, category, priority, notes, link
        FROM items
        WHERE user_id = ?
        ORDER BY id;
    )";

    sqlite3_stmt *stmt;
    if (!prepareStatement(sql, &stmt)) {
        return items;
    }

    sqlite3_bind_int(stmt, 1, userId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auto item = std::make_unique<WishItem>();
        item->setId(sqlite3_column_int(stmt, 0));
        // For now, we'll create items and they'll get new IDs
        // In production, you'd want to add a setId() method or use a factory
        std::string name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        double price = sqlite3_column_double(stmt, 2);
        bool purchased = sqlite3_column_int(stmt, 3) != 0;
        Category category = static_cast<Category>(sqlite3_column_int(stmt, 4));
        Priority priority = static_cast<Priority>(sqlite3_column_int(stmt, 5));

        const char *notesPtr = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));
        const char *linkPtr = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 7));

        std::string notes = notesPtr ? notesPtr : "";
        std::string link = linkPtr ? linkPtr : "";
        item->setName(name);
        item->setPrice(price);
        item->setPurchased(purchased);
        item->setCategory(category);
        item->setPriority(priority);
        item->setNotes(notes);
        item->setLink(link);

        items.push_back(std::move(item));
    }

    sqlite3_finalize(stmt);
    LOG_INFO("DatabaseHandler: Loaded ", items.size(), " items for user: ", owner);
    return items;
}

bool DatabaseHandler::saveBudget(const Budget &budget, const std::string &owner) {
    if (!createUser(owner)) {
        return false;
    }

    int userId = getUserId(owner);
    if (userId == -1) {
        return false;
    }

    const char *sql = R"(
        INSERT OR REPLACE INTO budgets
        (user_id, max_budget, spent_amount, enabled, updated_at)
        VALUES (?, ?, ?, ?, CURRENT_TIMESTAMP);
    )";

    sqlite3_stmt *stmt;
    if (!prepareStatement(sql, &stmt)) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_double(stmt, 2, budget.getMaxBudget());
    sqlite3_bind_double(stmt, 3, budget.getSpentAmount());
    sqlite3_bind_int(stmt, 4, budget.isEnabled() ? 1 : 0);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        LOG_ERROR("DatabaseHandler: Failed to save budget: ", sqlite3_errmsg(db));
        return false;
    }

    LOG_DEBUG("DatabaseHandler: Budget saved for user: ", owner);
    return true;
}

Budget DatabaseHandler::loadBudget(const std::string &owner) {
    Budget budget;

    int userId = getUserId(owner);
    if (userId == -1) {
        return budget;
    }

    const char *sql = "SELECT max_budget, spent_amount, enabled FROM budgets WHERE user_id = ?;";
    sqlite3_stmt *stmt;

    if (!prepareStatement(sql, &stmt)) {
        return budget;
    }

    sqlite3_bind_int(stmt, 1, userId);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        double maxBudget = sqlite3_column_double(stmt, 0);
        double spentAmount = sqlite3_column_double(stmt, 1);
        bool enabled = sqlite3_column_int(stmt, 2) != 0;

        budget.setMaxBudget(maxBudget);
        budget.setSpentAmount(spentAmount);
        if (enabled) {
            budget.enable();
        } else {
            budget.disable();
        }

        LOG_DEBUG("DatabaseHandler: Budget loaded for user: ", owner);
    }

    sqlite3_finalize(stmt);
    return budget;
}

int DatabaseHandler::getTotalItemsCount(const std::string &owner) {
    int userId = getUserId(owner);
    if (userId == -1) {
        return 0;
    }

    const char *sql = "SELECT COUNT(*) FROM items WHERE user_id = ?;";
    sqlite3_stmt *stmt;

    if (!prepareStatement(sql, &stmt)) {
        return 0;
    }
    sqlite3_bind_int(stmt, 1, userId);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count;
}

double DatabaseHandler::getTotalValue(const std::string &owner) {
    int userId = getUserId(owner);
    if (userId == -1) {
        return 0.0;
    }

    const char *sql = "SELECT SUM(price) FROM items WHERE user_id = ?;";
    sqlite3_stmt *stmt;

    if (!prepareStatement(sql, &stmt)) {
        return 0.0;
    }

    sqlite3_bind_int(stmt, 1, userId);

    double total = 0.0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        total = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return total;
}

bool DatabaseHandler::clearAllData(const std::string &owner) {
    int userId = getUserId(owner);
    if (userId == -1) {
        return false;
    }

    const char* sql = "DELETE FROM items WHERE user_id = ?;";
    sqlite3_stmt* stmt;

    if (!prepareStatement(sql, &stmt)) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, userId);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        return false;
    }

    LOG_INFO("DatabaseHandler: Cleared all data for user: ", owner);
    return true;
}

bool DatabaseHandler::vacuum() {
    LOG_INFO("DatabaseHandler: Running VACUUM to optimize database");
    return executeSQL("VACUUM;");
}

std::string DatabaseHandler::getLastError() const {
    if (db) {
        return sqlite3_errmsg(db);
    }
    return "Database not initialized";
}


