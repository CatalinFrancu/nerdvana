#include "Game.h"
#include "Piece.h"
#include <string>

bool Game::setType(std::string desc) {
  if (desc == "Blokus Duo") {
    restart(Board::T_DUO);
    return true;
  } else if (desc == "Blokus") {
    restart(Board::T_CLASSIC);
    return true;
  } else {
    return false;
  }
}

void Game::restart() {
  restart(board.type);
}

void Game::restart(int boardType) {
  board.init(boardType);
  Piece::precomputePieces(board.getSize());
}
