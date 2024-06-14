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

std::string Game::genMove(int player) {
  MoveList list;
  board.genMoves(player, list);
  assert(list.size);
  int m = board.chooseMove(player, list);
  board.makeMove(player, list.moves[m], list.pieceIndex[m]);
  return StrUtil::moveToString(list.moves[m], board.getSize());
}

void Game::makeMove(int player, std::string move) {
  bitset mask = StrUtil::stringToMove(move, board.getSize());
  int piece = board.getPieceFromMask(mask);
  board.makeMove(player, mask, piece);
}
