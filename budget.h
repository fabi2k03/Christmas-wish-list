//
// Created by Fabian Kopf on 20.11.25.
//

#ifndef CHISTMAS_WISHLIST_BUDGET_H
#define CHISTMAS_WISHLIST_BUDGET_H

#include <string>

using namespace std;

class Budget {
private:
    double maxBudget;
    double spentAmount;
    bool enabled;

public:
    Budget();
    Budget(double maxBudget);

    //Setters
    void setMaxBudget(double amount);
    void setSpentAmount(double amount);
    void enable();
    void disable();

    //Getters
    double getMaxBudget() const;
    double getSpentAmount() const;
    bool isEnabled() const;
    double getRemaining() const;
    double getSpentAmount() const;

    //Operations
    void addExpense(double amount);
    void removeExpense(double amount);
    void reset();

    //Checks
    bool isOverBudget() const;
    bool isNearLimit(double threshold = 0.8) const;
    string getStatusMessage() const;

    //Serialization
    string serialize() const;
    static Budget deserialize(string &data);
};


#endif //CHISTMAS_WISHLIST_BUDGET_H