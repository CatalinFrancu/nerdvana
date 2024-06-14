#pragma once

#include "Board.h"
#include <string>

class Game {
public:
  Board board;

  bool setType(std::string desc);
  void restart();
  void restart(int boardType);

  // Make and return a move for player. The player is assumed to still be in
  // the game.
  std::string genMoveFor(int player);
};
