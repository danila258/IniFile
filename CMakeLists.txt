cmake_minimum_required(VERSION 3.5)
project(IniFile)

set(CMAKE_CXX_STANDARD 11)

add_library(${PROJECT_NAME} STATIC src/IniFile.cpp src/IniSection.cpp)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
