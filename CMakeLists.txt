cmake_minimum_required(VERSION 3.1...3.24)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project(
        ModernCMakeExample
        VERSION 1.0
        LANGUAGES CXX)

add_executable(hex HexBoard.cpp)