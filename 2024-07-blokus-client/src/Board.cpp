#include "Board.h"
#include "globals.h"
#include "MoveList.h"

const int Board::SIZES[Board::NUM_TYPES] = { 14, 20 };
const int Board::NUM_PLAYERS[Board::NUM_TYPES] = { 2, 4 };
const int Board::STARTING_POSITIONS[Board::NUM_TYPES][Board::MAX_PLAYERS] = {
  { 380, 399, 19, 0 },
  { 130, 65 },
};

bitset Board::firstRank;
bitset Board::lastRank;
bitset Board::firstFile;
bitset Board::lastFile;

int Board::getSize() {
  return SIZES[type];
}

int Board::getNumPlayers() {
  return NUM_PLAYERS[type];
}

void Board::init(int type) {
  this->type = type;
  active = (1 << getNumPlayers()) - 1; // everybody
  toMove = 0;
  initBorderMasks();
  initPlayerMasks();
}

void Board::initBorderMasks() {
  int size = getSize();
  for (int i = 0; i < size; i++) {
    firstRank.set(i);
    lastRank.set(size * (size - 1) + i);
    firstFile.set(size * i);
    lastFile.set(size * i + size - 1);
  }
}

void Board::initPlayerMasks() {
  for (int i = 0; i < getNumPlayers(); i++) {
    occ[i] = 0;
    inHand[i] = (1 << NUM_PIECES) - 1;
  }
}

void Board::genMoves(MoveList& dest) {
  dest.size = 0;
  while (active && !dest.size) {
    genMovesForActivePlayer(dest);
    if (!dest.size) {
      deactivateActivePlayer();
      advancePlayer();
    }
  }
}

void Board::genMovesForActivePlayer(MoveList& dest) {
  int hand = inHand[toMove];
  while (hand) {
    int p = __builtin_ctz(hand);
    for (int rot = 0; rot < numRotations[p]; rot++) {
      genMovesWithPiece(p, rot, dest);
    }
    hand &= hand - 1;
  }
}

void Board::genMovesWithPiece(int p, int rot, MoveList& dest) {
  bitset& orig = pieces[p][rot];

  bool rankOverflow = false;
  int rank = 0;
  while (!rankOverflow) {
    bitset mask = orig << (rank * getSize());

    bool fileOverflow = false;
    int file = 0;
    while (!fileOverflow) {
      mask <<= 1;

      tryMove(p, mask, dest);

      file++;
      fileOverflow = (mask & lastFile).any();
    }

    rank++;
    rankOverflow = (mask & lastRank).any();
  }
}

void Board::tryMove(int p, bitset& mask, MoveList& dest) {
  bitset unavailable = 0;
  bitset& me = occ[toMove];
  int size = getSize();
  int np = getNumPlayers();

  // Squares occupied by any player are unavailable.
  for (int i = 0; i < np; i++) {
    unavailable |= occ[i];
  }

  // Squares adjacent to my pieces are unavailable.
  unavailable |= (me & ~firstRank) >> size;
  unavailable |= (me & ~lastRank) << size;
  unavailable |= (me & ~firstFile) >> 1;
  unavailable |= (me & ~lastFile) << 1;

  // The piece must touch a stepping stone.
  bitset stones =
    ((me & ~firstRank & ~firstFile) >> (size + 1)) |
    ((me & ~firstRank & ~lastFile) >> (size - 1)) |
    ((me & ~lastRank & ~firstFile) << (size - 1)) |
    ((me & ~lastRank & ~lastFile) << (size + 1));

  if ((mask & stones).any() && (mask & unavailable).none()) {
    dest.add(mask, p);
  }
}

void Board::deactivateActivePlayer() {
  active ^= (1 << toMove);
}

void Board::advancePlayer() {
  if (active) {
    do {
      toMove = (toMove + 1) % getNumPlayers();
    } while (!(active & (1 << toMove)));
  }
}
