project(test)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(gtest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


add_executable(runTests ../client.cpp ../client.h tests.cpp)

target_link_libraries(runTests gtest gtest_main)
