#include "assert.h"
#include "Board.h"
#include "globals.h"
#include "MoveList.h"

const int Board::SIZES[Board::NUM_TYPES] = { 14, 20 };
const int Board::NUM_PLAYERS[Board::NUM_TYPES] = { 2, 4 };
const int Board::STARTING_POSITIONS[Board::NUM_TYPES][Board::MAX_PLAYERS] = {
  { 130, 65 },
  { 380, 399, 19, 0 },
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

void Board::genMoves(int player, MoveList& dest) {
  bitset unavailable;
  bitset stones;
  makeLandscape(player, unavailable, stones);

  dest.size = 0;
  int hand = inHand[player];
  while (hand) {
    int piece = __builtin_ctz(hand);
    for (int rot = 0; rot < numRotations[piece]; rot++) {
      genMovesWithPiece(player, piece, rot, unavailable, stones, dest);
    }
    hand &= hand - 1;
  }
}

void Board::makeLandscape(int player, bitset& unavailable, bitset& stones) {
  unavailable.reset();
  bitset& me = occ[player];
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
  if (me.any()) {
    stones =
      ((me & ~firstRank & ~firstFile) >> (size + 1)) |
      ((me & ~firstRank & ~lastFile) >> (size - 1)) |
      ((me & ~lastRank & ~firstFile) << (size - 1)) |
      ((me & ~lastRank & ~lastFile) << (size + 1));
  } else {
    stones = getStartingPos(player);
  }
}

bitset Board::getStartingPos(int player) {
  bitset result(0);
  int pos = STARTING_POSITIONS[type][player];
  result[pos] = true;
  return result;
}

void Board::genMovesWithPiece(int player, int piece, int rot, bitset& unavailable,
                              bitset& stones, MoveList& dest) {
  bitset& orig = pieces[piece][rot];

  bool rankOverflow = false;
  int rank = 0;
  while (!rankOverflow) {
    bitset mask = orig << (rank * getSize());
    tryMove(piece, mask, unavailable, stones, dest);

    bool fileOverflow = false;
    int file = 0;
    while (!fileOverflow) {
      mask <<= 1;

      tryMove(piece, mask, unavailable, stones, dest);

      file++;
      fileOverflow = (mask & lastFile).any();
    }

    rank++;
    rankOverflow = (mask & lastRank).any();
  }
}

void Board::tryMove(int piece, bitset& mask, bitset& unavailable,
                    bitset& stones, MoveList& dest) {
  if ((mask & stones).any() && (mask & unavailable).none()) {
    dest.add(mask, piece);
  }
}

int Board::chooseMove(int player, MoveList& list) {
  int i = 0;
  while ((i < list.size) &&
         (PIECE_SIZES[list.pieceIndex[i]] == PIECE_SIZES[list.pieceIndex[0]])) {
    i++;
  }
  return rand() % i;
}

void Board::makeMove(int player, bitset& mask, int piece) {
  occ[player] |= mask;
  inHand[player] ^= (1 << piece);
}

int Board::getPieceFromMask(bitset mask) {
  // Shift the mask towards the LSB
  while ((mask & firstRank).none()) {
    mask >>= getSize();
  }
  while ((mask & firstFile).none()) {
    mask >>= 1;
  }

  for (int p = 0; p < NUM_PIECES; p++) {
    for (int rot = 0; rot < numRotations[p]; rot++) {
      if (pieces[p][rot] == mask) {
        return p;
      }
    }
  }
  assert(false);
}

void Board::print() {
  int size = getSize();
  for (int rank = size - 1; rank >= 0; rank--) {
    printf("%2d ", rank + 1);
    for (int file = 0; file < size; file++) {
      int bit = rank * size + file;
      printBit(bit);
    }
    printf("\n");
  }

  printf("   ");
  for (int file = 0; file < size; file++) {
    printf(" %c", file + 'a');
  }
  printf("\n");
}

void Board::printBit(int bit) {
  int p = 0;
  while ((p < getNumPlayers()) && !occ[p][bit]) {
    p++;
  }

  if (p == getNumPlayers()) {
    printf("  ");
  } else {
    printf("%s  %s", ANSI_COLORS[p], DEFAULT_COLOR);
  }
}
