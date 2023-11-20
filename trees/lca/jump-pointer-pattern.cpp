// Computes and prints the pattern of jump pointers used in the two pointer
// method.
#include <stdio.h>

const int N = 64;

int jump[N + 1];

int main() {

  jump[0] = 0;
  for (int i = 1; i <= N; i++) {
    int p = i - 1;
    int j2 = jump[p] - p;
    int j3 = jump[jump[p]] - jump[p];
    jump[i] = (j2 == j3) ? jump[jump[p]] : p;

    printf("jump[%2d] = %2d dist = %2d\n", i, jump[i], i - jump[i]);
  }

  return 0;
}

/**
jump[ 1] =  0 dist =  1
jump[ 2] =  1 dist =  1
jump[ 3] =  0 dist =  3
jump[ 4] =  3 dist =  1
jump[ 5] =  4 dist =  1
jump[ 6] =  3 dist =  3
jump[ 7] =  0 dist =  7
jump[ 8] =  7 dist =  1
jump[ 9] =  8 dist =  1
jump[10] =  7 dist =  3
jump[11] = 10 dist =  1
jump[12] = 11 dist =  1
jump[13] = 10 dist =  3
jump[14] =  7 dist =  7
jump[15] =  0 dist = 15
jump[16] = 15 dist =  1
jump[17] = 16 dist =  1
jump[18] = 15 dist =  3
jump[19] = 18 dist =  1
jump[20] = 19 dist =  1
jump[21] = 18 dist =  3
jump[22] = 15 dist =  7
jump[23] = 22 dist =  1
jump[24] = 23 dist =  1
jump[25] = 22 dist =  3
jump[26] = 25 dist =  1
jump[27] = 26 dist =  1
jump[28] = 25 dist =  3
jump[29] = 22 dist =  7
jump[30] = 15 dist = 15
jump[31] =  0 dist = 31
jump[32] = 31 dist =  1
jump[33] = 32 dist =  1
jump[34] = 31 dist =  3
jump[35] = 34 dist =  1
jump[36] = 35 dist =  1
jump[37] = 34 dist =  3
jump[38] = 31 dist =  7
jump[39] = 38 dist =  1
jump[40] = 39 dist =  1
jump[41] = 38 dist =  3
jump[42] = 41 dist =  1
jump[43] = 42 dist =  1
jump[44] = 41 dist =  3
jump[45] = 38 dist =  7
jump[46] = 31 dist = 15
jump[47] = 46 dist =  1
jump[48] = 47 dist =  1
jump[49] = 46 dist =  3
jump[50] = 49 dist =  1
jump[51] = 50 dist =  1
jump[52] = 49 dist =  3
jump[53] = 46 dist =  7
jump[54] = 53 dist =  1
jump[55] = 54 dist =  1
jump[56] = 53 dist =  3
jump[57] = 56 dist =  1
jump[58] = 57 dist =  1
jump[59] = 56 dist =  3
jump[60] = 53 dist =  7
jump[61] = 46 dist = 15
jump[62] = 31 dist = 31
jump[63] =  0 dist = 63
jump[64] = 63 dist =  1
 **/
