# 🎄Christmas Wishlist Manager
A feature-rich, multi-user command-line application for managing
Christmas wishlists, built with modern C++. Perfect for families,
gift coordinators, or anyone who want to organize their holiday shopping
efficiently.

## Features
### Core Funcionality
- **Add/Remove items** - Manage wishlist items with name, price, category and priority
- **Categories** - Organize items (Toys, Books, Electronics, Clothing, Sport, other)
- **Priority Level** - Set urgency (Low, Medium, High, Urgent)
- **Purchase Tracking** - Mark items as purchased/pending
- **Smart Search** - Find items by name with partial matching
- **Statistics** - View total value, purchased vs. pending, and more

### Advanced Features
- **Multi-user Support** - Each user has their own wishlist file
- **User switching** - Switch between users with ease
- **Persistent Storage** - Automatic save/load with ````.dat```` files
- **CSV Export/Import** - Share wishlists via csv format
- **Multiple Sort Options** - Sort by price, name, category, priority or ID
- **Notes & Links** - Add detailed notes and product URLs

### Budget Management
A brand-new budgeting system helps you stay financially safe while planning your Christmas shopping.
- **Enable/Disable Budget** - Turn budget tracking on or off
- **Set Maximum Budget** - Define how much you want to spend
- **Automatic Spent Tracking** - Every purchased item update the budget
- **Remaining Budget Calculation** - Know instantly how much money you have left
- **Near Limit Warnings** - Receive alerts when reaching 80-90% of the budget
- **Over-Budget-Detection** - Clear warning when spending exceeds the budget
- ...

---

## 🚀 Installation
### Prerequisites
- C++17 compatible compiler (g++ 7+, clang++ 5+, MSVC 2017+)
- CMake 3.10+ (optional, for build system)
- Standard C++ libraries

### Quick Start 
#### macOS/Linux
```bash
# Clone the repository
git clone https://github.com/fabi2k03/Christmas-wish-list.git
cd Christmas-wish-list

# Compile
g++ -std=c++17 -o wishlist main.cpp wishlist.cpp wishlist_manager.cpp file_handler.cpp utils.cpp

# Run
./wishlist
```

#### Using CMake
```bash
# Create build directory
mkdir build && cd build

# Generate build files
cmake ..

# Build
cmake --build .

# Run
./Chistmas_WishList
```

#### Windows
```bash
git clone https://github.com/fabi2k03/Christmas-wish-list.git
cd Christmas-wish-list

# Open in Visual Studio or use command line:
cl /EHsc /std:c++17 main.cpp wishlist.cpp wishlist_manager.cpp file_handler.cpp utils.cpp /Fe:wishlist.exe

# Run
wishlist.exe
```
<div align="center">
🎄 Happy Holidays! 🎅
Made with ❤️ and C++
</div>