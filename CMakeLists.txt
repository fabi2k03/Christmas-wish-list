cmake_minimum_required(VERSION 3.20)
project(Chistmas_WishList)

set(CMAKE_CXX_STANDARD 20)

# Enable testing
enable_testing()

#Add Google Test
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Chistmas_WishList main.cpp
        wishlist.h
        wishlist.cpp
        wishlist_manager.h
        wishlist_manager.cpp
        file_handler.h
        file_handler.cpp
        utils.h
        utils.cpp
        logger.h
        logger.cpp)

set(SOURCE_FILES
        wishlist.cpp
        wishlist_manager.cpp
        file_handler.cpp
        utils.cpp
        logger.cpp
)

# Test executable
add_executable(wishlist_tests
        tests/test_wishitem.cpp
        tests/test_wishlist_manager.cpp
        tests/test_file_handler.cpp
        #tests/test_utils.cpp
        ${SOURCE_FILES}
)

# Link Google Test
target_link_libraries(wishlist_tests
        GTest::gtest_main
)

# For macOS/Linux - Threading support
if(UNIX)
    target_link_libraries(Chistmas_WishList pthread)
    target_link_libraries(wishlist_tests pthread)
endif()

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(wishlist_tests)
