cmake_minimum_required(VERSION 3.0.0)
project(TestCode VERSION 0.1.0 LANGUAGES C CXX)

INCLUDE_DIRECTORIES (
    C:/dev/vcpkg/installed/x64-windows/include
)

LINK_DIRECTORIES (
    C:/dev/vcpkg/installed/x64-windows/lib
)

add_executable(TestCode main.cpp)

TARGET_LINK_LIBRARIES (TestCode sqlite3 lsl)
