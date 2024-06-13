#include "MoveList.h"

void MoveList::add(bitset& mask, int piece) {
  moves[size] = mask;
  pieceIndex[size] = piece;
  size++;
}
