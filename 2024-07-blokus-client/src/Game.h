#pragma once

#include "Board.h"
#include <string>

class Game {
public:
  Board board;

  bool setType(std::string desc);
  void restart();
  void restart(int boardType);

  // Find, make and return a move for player. The player is assumed to still
  // be in the game.
  std::string genMove(int player);

  void makeMove(int player, std::string move);
};
