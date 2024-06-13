#pragma once

#include "globals.h"

class Piece {
public:
  bitset masks[MAX_ROTATIONS];

  static void precomputePieces(int boardSize);

private:
  static void copyPieceMap(const pieceMap src, pieceMap dest);
  static void pieceMapToBitset(pieceMap pm, int index, int boardSize);
  static void rotatePieceMap(pieceMap pm);
  static void mirrorPieceMap(pieceMap pm);
};
