cmake_minimum_required(VERSION 4.0)
project(Chistmas_WishList)

set(CMAKE_CXX_STANDARD 20)

add_executable(Chistmas_WishList main.cpp
        wishlist.h
        wishlist.cpp
        wishlist_manager.h
        wishlist_manager.cpp
        file_handler.h
        file_handler.cpp
        utils.h
        utils.cpp)
