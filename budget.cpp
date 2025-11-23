//
// Created by Fabian Kopf on 20.11.25.
//

#include "budget.h"
#include "logger.h"
#include <iomanip>
#include <sstream>
#include <cmath>
#include <vector>

using namespace std;

Budget::Budget() : maxBudget(0.0), spentAmount(0.0), enabled(false) {
    LOG_DEBUG("Budget: Default constructor called");
}

Budget::Budget(double maxBudget) : maxBudget(maxBudget), spentAmount(0.0), enabled(true) {
    LOG_INFO("Budget: Created with max budget: ", maxBudget);
}

void Budget::setMaxBudget(double amount) {
    if (amount < 0) {
        LOG_WARNING("Budget: Attempting to set negative budget: ", amount);
        return;
    }
    maxBudget = amount;
    enabled = true;
    LOG_INFO("Budget: Set max budget to: ", amount);
}

void Budget::setSpentAmount(double amount) {
    if (amount < 0) {
        LOG_WARNING("Budget: Attempting to set negative spent amount: ", amount);
        return;
    }
    spentAmount = amount;
    LOG_DEBUG("Budget: Spent amount set to: ", amount);
}

void Budget::enable() {
    enabled = true;
    LOG_INFO("Budget: Enabled");
}

void Budget::disable() {
    enabled = false;
    LOG_INFO("Budget: Disabled");
}

double Budget::getMaxBudget() const {
    return maxBudget;
}

double Budget::getSpentAmount() const {
    return spentAmount;
}

bool Budget::isEnabled() const {
    return enabled;
}

double Budget::getRemaining() const {
    return maxBudget - spentAmount;
}

double Budget::getSpendingPercentage() const {
    if (maxBudget <= 0) {
        return 0.0;
    }
    return (spentAmount / maxBudget) * 100;
}

void Budget::addExpense(double amount) {
    if (amount < 0) {
        LOG_WARNING("Budget: Attempted to add negativ expsense: ", amount);
        return;
    }
    spentAmount += amount;
    LOG_DEBUG("Budget: Added expense: ", amount, ", Total spent: ", spentAmount);

    if (isNearLimit() && !isOverBudget()) {
        LOG_WARNING("Budget: Approaching limit (", getSpendingPercentage(), "%");
    }
    if (isOverBudget()) {
        LOG_WARNING("Budget: Over budget! Spent: ", spentAmount, ", Limit: ", maxBudget);
    }
}

void Budget::removeExpense(double amount) {
    if (amount < 0) {
        LOG_WARNING("Budget: Attempted to remove negativ expsense: ", amount);
        return;
    }
    spentAmount -= amount;
    if (spentAmount < 0) {
        spentAmount = 0;
    }
    LOG_DEBUG("Budget: Remove expense: ", amount, ", Total spent: ", spentAmount);
}

void Budget::reset() {
    spentAmount = 0.0;
    LOG_INFO("Budget: Reset spent amount to 0");
}

bool Budget::isOverBudget() const {
    if (!enabled || maxBudget <= 0) {
        return false;
    }
    return spentAmount > maxBudget;
}

bool Budget::isNearLimit(double threshold) const {
    if (!enabled || maxBudget <= 0) {
        return false;
    }
    return (spentAmount / maxBudget) >= threshold;
}

string Budget::getStatusMessage() const {
    if (!enabled) {
        return "Budget tracking is disabled";
    }

    stringstream ss;
    ss << fixed << setprecision(2);

    if (isOverBudget()) {
        double overage = spentAmount - maxBudget;
        ss << "OVER BUDGET by €  " << overage << "!";
    } else if (isNearLimit(0.9)) {
        ss << "Warning: " << getSpendingPercentage() << "% of budget used";
    } else {
        ss << "Within budget (" << getSpendingPercentage() << "%)";
    }

    return ss.str();
}

string Budget::serialize() const {
    stringstream ss;
    ss << fixed << std::setprecision(2);
    ss << maxBudget << "|" << spentAmount << "|" << enabled;
    return ss.str();
}

Budget Budget::deserialize(string &data) {
    Budget budget;
    stringstream ss(data);
    string token;
    vector<string> tokens;

    while (getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.size() >= 3) {
        try {
            budget.maxBudget = stod(tokens[0]);
            budget.spentAmount = stod(tokens[1]);
            budget.enabled = (tokens[2] == "1");
            LOG_DEBUG("Budget: Deserialized - Max: ", budget.maxBudget, ", Spent: ", budget.spentAmount);
        } catch (const exception &e) {
            LOG_ERROR("Budget: Deserialziation failed - ", e.what());
        }
    }
    return budget;
}
