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
