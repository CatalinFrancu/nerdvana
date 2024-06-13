#pragma once

#include "globals.h"

class MoveList {
public:
  // TBD what is reasonable
  static const int MAX_MOVES = 1'000;

  // Store the piece mask, but also which piece it was, so that we can remove
  // it from the player's hand.
  bitset moves[MAX_MOVES];
  char pieceIndex[MAX_MOVES];

  int player;
  int size;

  void add(bitset& mask, int piece);
};
