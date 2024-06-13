#pragma once

#include "Board.h"
#include <string>

class Game {
public:
  Board board;

  bool setType(std::string desc);
  void restart();
  void restart(int boardType);
};
