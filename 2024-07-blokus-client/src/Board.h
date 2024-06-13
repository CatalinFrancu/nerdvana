#pragma once

#include "globals.h"
#include "MoveList.h"

class Board {
public:
  static const int MAX_PLAYERS = 4;

  static const int T_DUO = 0;
  static const int T_CLASSIC = 1;
  static const int NUM_TYPES = 2;
  static const int SIZES[];
  static const int NUM_PLAYERS[];
  static const int STARTING_POSITIONS[][MAX_PLAYERS];

  unsigned char type;   // 2-player Duo or 4-player Classic
  unsigned char active; // 4-bit mask of players that are still active
  unsigned char toMove; // side to move, [0...numPlayers)

  // border masks to be used during the move generation
  static bitset firstRank, lastRank, firstFile, lastFile;

  // occupancy per player
  bitset occ[MAX_PLAYERS];

  // bit masks of pieces still in hand
  int inHand[MAX_PLAYERS];

  int getSize();
  int getNumPlayers();
  void init(int type);
  void genMoves(MoveList& dest);

private:
  void initBorderMasks();
  void initPlayerMasks();
  void genMovesForActivePlayer(MoveList& dest);
  void genMovesWithPiece(int p, int rot, MoveList& dest);
  void tryMove(int p, bitset& mask, MoveList& dest);
  void deactivateActivePlayer();
  void advancePlayer();
};
