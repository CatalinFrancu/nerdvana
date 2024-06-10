#pragma once

#include <string>

class Game {
public:
  const int T_DUO = 0;
  const int T_CLASSIC = 1;

  int type;

  bool setType(std::string desc);
  void restart();
};
