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

  static constexpr char ANSI_COLORS[MAX_PLAYERS][7] = {
    "\e[106m", "\e[103m", "\e[101m", "\e[102m",
  };
  static constexpr char DEFAULT_COLOR[7] = "\e[49m";

  unsigned char type;   // 2-player Duo or 4-player Classic

  // border masks to be used during the move generation
  static bitset firstRank, lastRank, firstFile, lastFile;

  // occupancy per player
  bitset occ[MAX_PLAYERS];

  // bit masks of pieces still in hand
  int inHand[MAX_PLAYERS];

  int getSize();
  int getNumPlayers();
  void init(int type);
  void genMoves(int player, MoveList& dest);
  int chooseMove(int player, MoveList& list);
  void makeMove(int player, bitset& mask, int piece);
  int getPieceFromMask(bitset mask);
  void print();
  void printBit(int bit);

private:
  void initBorderMasks();
  void initPlayerMasks();
  void makeLandscape(int player, bitset& unavailable, bitset& stones);
  bitset getStartingPos(int player);
  void genMovesWithPiece(int player, int piece, int rot, bitset& unavailable,
                         bitset& stones, MoveList& dest);
  void tryMove(int piece, bitset& mask, bitset& unavailable,
               bitset& stones, MoveList& dest);
};
