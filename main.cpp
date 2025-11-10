#include <iostream>

void displayMenu() {
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║   🎄 CHRISTMAS WISHLIST MANAGER 🎄  ║\n";
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

int main() {
    displayMenu();
    return 0;
}