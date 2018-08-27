 /* Those Combinatorial Algorithms taken from TAOCP Volume 4A by D. Knuth */
#include <stdio.h>

#define n 1647
#define t 4

long int count = 0;

void comb3() {
  int c1, c2, c3;
  for (c3 = 2; c3 <= n - 1; c3 += 1) {
    for (c2 = 1; c2 <= c3 - 1; c2 += 1) {
      for (c1 = 0; c1 <= c2 - 1; c1 += 1) {
        count++; if (count % 1000000000 == 0) printf("%lu\n", count);
        /*printf("%i %i %i\n", c3, c2, c1);*/
      }
    }
  }
}

void comb4() {
  int c1, c2, c3, c4;
  for (c4 = 3; c4 <= n - 1; c4 += 1) {
    for (c3 = 2; c3 <= c4 - 1; c3 += 1) {
      for (c2 = 1; c2 <= c3 - 1; c2 += 1) {
        for (c1 = 0; c1 <= c2 - 1; c1 += 1) {
          count++; if (count % 1000000000 == 0) printf("%lu\n", count);
          /*printf("%i %i %i %i\n", c4, c3, c2, c1);*/
        }
      }
    }
  }
}

void combR(int start, int end, int c[], int size) {
  if (start >= 0) {
    int i = 0;
    for (i = start; i <= end; i += 1) {
      c[start + 1] = i;
      combR(start - 1, i - 1, c, size);
    }
  } else {
    count++; if (count % 100000000 == 0) printf("%lu\n", count);
    /*
    int i;
    for (i = 1; i <= size; i += 1) {
      printf("%i ", c[i]);
    }
    printf("\n");
    */
  }
}

void combL() {
  int c[t+3];
  int j;
  for (j = 1; j <= t; j += 1) {
    c[j] = j -1;
  }
  c[t+1] = n;
  c[t+2] = 0;

  while (1) {
    /*
    for(j = t; j >= 1; j -= 1) {
      printf("%i ", c[j]);
    }
    printf("\n");
    */
    count++; if (count % 1000000000 == 0) printf("%lu\n", count);

    j = 1;
    while(c[j] + 1 == c[j+1]) {
      c[j] = j - 1;
      j += 1;
    }

    if (j > t) break;

    c[j] = c[j] + 1;
  }
}

void combT() {
  int c[t+3];
  int j, x;
  for (j = 1; j <= t; j += 1) {
    c[j] = j -1;
  }
  c[t+1] = n;
  c[t+2] = 0;
  j = t;

  while (1) {
T2: while (c[j+1] > j) {
      j -= 1;
    }

    int k;
    for(k = t; k >= 1; k -= 1) {
      printf("%i ", c[k]);
    }
    printf("\n");

    if (j > 0) {
      x = j;
      goto T6;
    }

T3: if (c[1] + 1 < c[2]) {
      c[1] += 1;
      goto T2;
    } else {
      j = 2;
    }

T4: c[j-1] = j - 2; x = c[j] + 1;
    if (x == c[j+1]) {
      j += 1;
      goto T4;
    }

    if (j > t) break;

T6: c[j] = x;
    j -= 1;
  }
}

int main() {
  /*
  int c[t+1];
  combR(t-1, n-1, c, t);
  */
  combL();
  printf("%lu\n", count);
}
