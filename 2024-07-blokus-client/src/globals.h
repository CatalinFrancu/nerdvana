#pragma once

#include <bitset>

const int MAX_CELLS = 400;
typedef std::bitset<MAX_CELLS> bitset;

const int NUM_PIECES = 21;
const int MAX_ROTATIONS = 8;
const int MAX_PIECE_SIZE = 5;
typedef char pieceMap[MAX_PIECE_SIZE + 1][MAX_PIECE_SIZE + 1];
const pieceMap PIECE_MAPS[NUM_PIECES] = {
  {
    "*..",
    "*..",
    "***",
  }, {
    "*.",
    "*.",
    "*.",
    "**",
  }, {
    "**.",
    ".*.",
    ".**",
  }, {
    ".*",
    ".*",
    "**",
    "*.",
  }, {
    "*..",
    "**.",
    ".**",
  }, {
    ".*.",
    "***",
    ".*.",
  }, {
    ".**",
    "**.",
    ".*.",
  }, {
    "*",
    "*",
    "*",
    "*",
    "*",
  }, {
    "***",
    ".*.",
    ".*.",
  }, {
    ".*",
    "**",
    ".*",
    ".*",
  }, {
    "**",
    "**",
    "*.",
  }, {
    "*.*",
    "***",
  }, {
    "*.",
    "*.",
    "**",
  }, {
    "*",
    "*",
    "*",
    "*",
  }, {
    "***",
    ".*.",
  }, {
    "**.",
    ".**",
  }, {
    "**",
    "**",
  }, {
    "*.",
    "**",
  }, {
    "*",
    "*",
    "*",
  }, {
    "**",
  }, {
    "*",
  },
};

extern bitset pieces[NUM_PIECES][MAX_ROTATIONS];
extern int numRotations[NUM_PIECES];
