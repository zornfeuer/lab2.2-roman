cmake_minimum_required(VERSION 3.14)
project(lab2 CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.0
)
FetchContent_MakeAvailable(Catch2)


# add_subdirectory(src)

# add_executable(lab main.cpp)

# target_link_libraries(lab PRIVATE
#     dyn_arr_lib
# )

add_executable(tests
    tests/dyn_arr_test.cpp
    tests/linked_list_test.cpp
)

target_include_directories(tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(tests PRIVATE
    Catch2::Catch2WithMain
)


enable_testing()

include(Catch)
catch_discover_tests(tests)