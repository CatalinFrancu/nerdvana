#include "Game.h"
#include <string>

bool Game::setType(std::string desc) {
  if (desc == "Blokus Duo") {
    type = T_DUO;
    restart();
    return true;
  } else if (desc == "Blokus") {
    type = T_CLASSIC;
    restart();
    return true;
  } else {
    return false;
  }
}

void Game::restart() {
}
