#pragma once

#include "globals.h"
#include <string>

class StrUtil {
public:
  static void reverse(char* s);
  static std::string moveToString(bitset& mask, int size);
};
