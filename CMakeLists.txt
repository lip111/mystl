cmake_minimum_required(VERSION 3.16)
project(mystl LANGUAGES CXX)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 默认构建类型：Debug(带调试信息-g)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "compile mode: debug")
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_compile_options(-g)
elseif(CMAKE_BUILD_TYPE STREQUAL Release)
    add_compile_options(-o2)
endif()

# 头文件搜索路径
include_directories(
    ${PROJECT_SOURCE_DIR}/alloc
    ${PROJECT_SOURCE_DIR}/container
)

file(GLOB TEST_FILES test/*.cpp)

# 生成测试可执行程序
add_executable(
    mystl_test ${TEST_FILES}
)