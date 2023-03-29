cmake_minimum_required(VERSION 3.5)
project(IniFile)

set(CMAKE_CXX_STANDARD 11)

add_library(${PROJECT_NAME} INTERFACE IniFile.h IniSection.h)
