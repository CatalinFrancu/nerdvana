cmake_minimum_required(VERSION 3.29)

project(
  blokus-client
  VERSION 1.0
  LANGUAGES CXX)

#include(GNUInstallDirs)

set(SOURCES
    src/Board.cpp
    src/Board.h
    src/Command.cpp
    src/Command.h
    src/Game.cpp
    src/Game.h
    src/globals.cpp
    src/globals.h
    src/main.cpp
    src/MoveList.cpp
    src/MoveList.h
    src/Piece.cpp
    src/Piece.h
    src/Response.cpp
    src/Response.h
    src/StrUtil.cpp
    src/StrUtil.h
)

add_executable(${PROJECT_NAME} ${SOURCES})
