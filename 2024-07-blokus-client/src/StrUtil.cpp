#include <string>
#include <string.h>
#include "StrUtil.h"

void StrUtil::reverse(char* s) {
  int i = 0, j = strlen(s) - 1;
  while (i < j) {
    char tmp = s[i];
    s[i] = s[j];
    s[j] = tmp;
    i++;
    j--;
  }
}

// TODO: This is O(n), should be O(number of bits set)
std::string StrUtil::moveToString(bitset& mask, int size) {
  std::string result = "";
  for (int rank = 0; rank < size; rank++) {
    for (int file = 0; file < size; file++) {
      if (mask[rank * size + file]) {
        if (result > "") {
          result += ',';
        }
        result += (file + 'a');
        result += std::to_string(rank + 1);
      }
    }
  }
  return result;
}

bitset StrUtil::stringToMove(std::string& move, int size) {
  bitset mask(0);
  int p = 0, len = move.length();
  while (p < len) {
    int file = move[p++] - 'a';
    int rank = 0;
    while (std::isdigit(move[p])) {
      rank = rank * 10 + (move[p++] - '0');
    }
    rank--; // 0-based
    mask[rank * size + file] = true;
    if (p < len) {
      p++; // skip the comma
    }
  }
  return mask;
}
