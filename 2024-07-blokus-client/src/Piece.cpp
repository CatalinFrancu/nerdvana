#include "Piece.h"
#include <string.h>
#include "StrUtil.h"

void Piece::precomputePieces(int boardSize) {
  pieceMap pm;
  for (int i = 0; i < NUM_PIECES; i++) {
    numRotations[i] = 0;
    copyPieceMap(PIECE_MAPS[i], pm);
    for (int mir = 0; mir < 2; mir++) {
      for (int rot = 0; rot < 4; rot++) {
        pieceMapToBitset(pm, i, boardSize);
        rotatePieceMap(pm);
      }
      mirrorPieceMap(pm);
    }
  }
}

void Piece::copyPieceMap(const pieceMap src, pieceMap dest) {
  int r =0;
  while (src[r][0]) {
    strcpy(dest[r], src[r]);
    r++;
  }
  dest[r][0] = '\0';
}

void Piece::pieceMapToBitset(pieceMap pm, int index, int boardSize) {
  bitset& bs = pieces[index][numRotations[index]];
  bs = 0;
  for (int r = 0; pm[r][0]; r++) {
    for (int c = 0; pm[r][c]; c++) {
      if (pm[r][c] == '*') {
        bs.set(r * boardSize + c);
      }
    }
  }

  // Check if the new bitset is distinct from all the others
  int i = 0;
  while ((i < numRotations[index]) && (bs != pieces[index][i])) {
    i++;
  }
  if (i == numRotations[index]) {
    numRotations[index]++;
  }
}

void Piece::rotatePieceMap(pieceMap pm) {
  pieceMap tmp;
  int rows = 0, cols = strlen(pm[0]);
  while (pm[rows][0]) {
    rows++;
  }

  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      tmp[cols - 1 - c][r] = pm[r][c];
    }
  }
  for (int c = 0; c < cols; c++) {
    tmp[c][rows] = '\0';
  }
  tmp[cols][0] = '\0';

  copyPieceMap(tmp, pm);
}

void Piece::mirrorPieceMap(pieceMap pm) {
  for (int r = 0; pm[r][0]; r++) {
    StrUtil::reverse(pm[r]);
  }
}
