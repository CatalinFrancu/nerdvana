#include <assert.h>
#include "Game.h"
#include "Piece.h"
#include <string>
#include "StrUtil.h"

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

std::string Game::genMoveFor(int player) {
  MoveList list;
  board.genMoves(player, list);
  assert(list.size);
  board.makeMove(player, list.moves[0], list.pieceIndex[0]);
  return StrUtil::moveToString(list.moves[0], board.getSize());
}
