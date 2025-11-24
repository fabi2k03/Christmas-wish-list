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

add_executable(Chistmas_WishList src/main.cpp
        include/wishlist.h
        src/wishlist.cpp
        include/wishlist_manager.h
        src/wishlist_manager.cpp
        include/file_handler.h
        src/file_handler.cpp
        include/utils.h
        src/utils.cpp
        include/logger.h
        src/logger.cpp
        src/budget.cpp
        include/budget.h
)

set(SOURCE_FILES
        src/wishlist.cpp
        src/wishlist_manager.cpp
        src/file_handler.cpp
        src/utils.cpp
        src/logger.cpp
        src/budget.cpp
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
