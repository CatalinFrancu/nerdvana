cmake_minimum_required(VERSION 3.29)

project(
  blokus-client
  VERSION 1.0
  LANGUAGES CXX)

#include(GNUInstallDirs)

set(SOURCES
    src/Command.h
    src/Command.cpp
    src/Game.h
    src/Game.cpp
    src/main.cpp
    src/Response.h
    src/Response.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
