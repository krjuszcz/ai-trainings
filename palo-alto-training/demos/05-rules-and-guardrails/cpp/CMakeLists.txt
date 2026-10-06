cmake_minimum_required(VERSION 3.16)
project(demo05_rules_and_guardrails CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W4 /WX /EHsc)
else()
    add_compile_options(-Wall -Wextra -Wconversion -Werror)
endif()

add_library(retry_policy src/retry_policy.cpp)
target_include_directories(retry_policy PUBLIC include)

add_executable(retry_policy_test tests/retry_policy_test.cpp)
target_link_libraries(retry_policy_test PRIVATE retry_policy)

enable_testing()
add_test(NAME retry_policy_test COMMAND retry_policy_test)
