/* compile with:
 * clang -std=c89 -lcrypto find-hash.c -o find-hash
 * Run with:
 * ./find-hash wordlist
 *
 * printout stout yawls
 * ty outlaws printouts
 * wu lisp not statutory
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

/* TODO: HASH_SIZE should be only characters in the anagram not the whole alphabet */
#define HASH_SIZE 26
#define WORD_LENGTH 25
#define ANAGRAM_LENGTH 23
#define MAX_POSSIBLE_WORDS 2000
#define COMB3_ITERATIONS 743255415 /* = (1647 x 1646 x 1645) / (3 x 2) */
#define COMB4_ITERATIONS 305477975565 /* = (1647 x 1646 x 1645 x 1644) / (4 x 3 x 2) */

long int count = 0;

struct word {
  unsigned short int len;
  unsigned short int hash [HASH_SIZE];
  char value[];
};

int getHash(const char * word, unsigned short int * const hash) {
  int i;
  for (i = 0; i < strlen(word); i += 1) {
    char c = word[i];
    if (c == ' ') continue; /* Skip space character */
    int index = c - 97;
    if (index < 0 || index > 25) return 0;
    hash[index] = hash[index] != 0 ? hash[index] + 1 : 1;
  }
  return 1;
}

int isValidHash(const char * word, const unsigned short int * knownHash) {
  unsigned short int hash[HASH_SIZE] = {0};
  int canHash = getHash(word, hash);
  int i = 0;
  for (i = 0; i < HASH_SIZE; i += 1) {
    if (canHash == 0 || hash[i] > knownHash[i]) {
      return 0;
    }
  }

  return 1;
}

int isKnownWord(struct word * words[], const char * newWord, int possibleWordsCount) {
  int i = 0;
  for (i = 0; i < possibleWordsCount; i += 1) {
    if (strcmp(words[i]->value, newWord) == 0) {
      return 1;
    }
  }
  return 0;
}

int getPossibleWord(const char * filepath, unsigned short int * knownAnagramHash, struct word * words[]) {
  FILE * inputFd;
  char line[WORD_LENGTH];
  int wordIndex = -1;
  inputFd = fopen(filepath, "r");
  if (inputFd == NULL) {
    printf("opening file %s failed\n", filepath);
    return 0;
  }

  while(fgets(line, WORD_LENGTH, inputFd) != NULL) {
    /* Strip LF & CRLF */
    line[strcspn(line, "\r\n")] = '\0';
    if (*line == '\0') continue;

    unsigned int len = strlen(line);

    if (len == 1) continue; /* Skip one char lines */

    int isMyHashValid = isValidHash(line, knownAnagramHash);

    if (isMyHashValid == 0) continue;

    int isMyWordKnown = isKnownWord(words, line, wordIndex + 1);

    if (isMyWordKnown == 1) continue;

    wordIndex +=1;
    strcpy(words[wordIndex]->value, line);
    words[wordIndex]->len = len;
    getHash(line, words[wordIndex]->hash);
  }

  if (fclose(inputFd) == -1) {
    printf("close input file %s failed\n", filepath);
    return 0;
  }

  return wordIndex + 1;
}


void md5sum(unsigned char anagram[], int len, char md5[]) {
  unsigned char md5sum[MD5_DIGEST_LENGTH];
  MD5(anagram, len, md5sum);

  int i;
  for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
    sprintf(&md5[i*2], "%02x", (unsigned int)md5sum[i]);
  }
}

int computeAnagram(struct word * const words[], const int indices[], const int wordsCount, unsigned char anagram[]) {
  int i, j, k = 0;
  for (i = 1; i <= wordsCount; i += 1) {
    for (j = 0; j < words[indices[i]]->len; j += 1) {
      anagram[k + j] = words[indices[i]]->value[j];
    }
    k += j;
    anagram[k++] = ' ';
  }
  anagram[--k] = '\0';

  return k;
}

/* TODO: Check other algorithm for generating permutation. This one called
 * heap recursive https://en.wikipedia.org/wiki/Heap%27s_algorithm */
void checkAnagrams(struct word * words[], int n, int c[], int size) {
  if (n == 1) {
    unsigned char anagram[ANAGRAM_LENGTH] = {0};
    int anagramLen = computeAnagram(words, c, size, anagram);

    char md5[33];
    md5sum(anagram, anagramLen, md5);

    if ( strcmp(md5, "e4820b45d2277f3844eac66c903e84be") != 0 &&
         strcmp(md5, "23170acc097c24edb98fc5488ab033fe") != 0 &&
         strcmp(md5, "665e5bcb0c20062fe8abaaf4628bb154") != 0 ) return;

    printf("solution: %s\n", anagram);
  } else {
    int m;
    for (m = 1; m < n; m += 1) {
      checkAnagrams(words, n-1, c, size);
      if (n % 2 == 0) {
        int tmp = c[m];
        c[m] = c[n];
        c[n] = tmp;
      } else {
        int tmp = c[1];
        c[1] = c[n];
        c[n] = tmp;
      }
    }
    checkAnagrams(words, n-1, c, size);
  }
}

/* TODO: Explore other combinatorics algorithms from D. Knuth Volume 4A book
 * This algorithm was simply the ineffient nested loops he mentioned in 7.2.1.3 (15)
 * But I made it recursive to allow both comb3 & comb4
 * Check: combinatorial-algos.c
 */
void combR(int start, int end, int c[], int size, struct word * words[],
    const unsigned short int * knownHash) {

  count++;
  if (count % 10000000 == 0) {
    printf("iteration: %lu out roughly %lu \n", count, size == 3 ? COMB3_ITERATIONS :
        COMB4_ITERATIONS);
  }

  if (start >= 0) {
    int i;
    for (i = start; i <= end; i += 1) {
      c[start + 1] = i;

      /* TODO: Explore possibly returning early if current comb is not sub-anagram
       * TODO: Check the possiblity of checking anagram hash using SIMD
       */

      int m, len = 0;
      for (m = size; m >= start + 1; m -= 1) {
        len += words[c[m]]->len;
      }
      if (len > ANAGRAM_LENGTH) return;

      combR(start - 1, i - 1, c, size, words, knownHash);
    }
  } else {
    /* Check if anagram */
    unsigned short int hash[HASH_SIZE] = {0};
    int m = 0;

    /* TODO: Check possibility to use SIMD */
    for(m = 1; m <= size; m += 1) {
      hash[0]  += words[c[m]]->hash[0]; /* letter: a */
      hash[8]  += words[c[m]]->hash[8];  /* letter: i */
      hash[11] += words[c[m]]->hash[11];  /* letter: l */
      hash[13] += words[c[m]]->hash[13];  /* letter: n */
      hash[14] += words[c[m]]->hash[14];  /* letter: o */
      hash[15] += words[c[m]]->hash[15];  /* letter: p */
      hash[17] += words[c[m]]->hash[17];  /* letter: r */
      hash[18] += words[c[m]]->hash[18];  /* letter: s */
      hash[19] += words[c[m]]->hash[19];  /* letter: t */
      hash[20] += words[c[m]]->hash[20];  /* letter: u */
      hash[22] += words[c[m]]->hash[22];  /* letter: w */
      hash[24] += words[c[m]]->hash[24];  /* letter: y */
    }

    if (memcmp(hash, knownHash, HASH_SIZE) != 0) return;

    int newC[size+1];
    for(m = 1; m <= size; m += 1) {
      newC[m] = c[m];
    }
    checkAnagrams(words, size, newC, size);
  }
}

void allocateWords(struct word * words[]) {
  int i = 0, j = 0;
  for (i = 0; i < MAX_POSSIBLE_WORDS; i += 1) {
    words[i] = malloc(WORD_LENGTH * sizeof(char) + (HASH_SIZE + 1) * sizeof(int));
    strcpy(words[i]->value, "\0");
    words[i]->len = 0;
    for (j = 0; j < HASH_SIZE; j += 1) {
      words[i]->hash[j] = 0;
    }
  }
}

void releaseWords(struct word * words[]) {
  int i =0;
  for (i = 0; i < MAX_POSSIBLE_WORDS; i += 1) {
    free(words[i]);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    printf("Usage: %s worldlist\n", argv[0]);
    return 1;
  }

  const char * knownAnagram = "poultry outwits ants";
  unsigned short int knownAnagramHash[HASH_SIZE] = {0};
  getHash(knownAnagram, knownAnagramHash);

  struct word * words[MAX_POSSIBLE_WORDS];
  allocateWords(words);

  const char * filepath = argv[1];
  int possibleWordsCount = getPossibleWord(filepath, knownAnagramHash, words);

  if (possibleWordsCount == 0) {
    printf("No possible words found!\n");
    return 1;
  }

  printf("Found %i possible words\n", possibleWordsCount);
  printf("It will take roughly %i iteration to do comb3\n", COMB3_ITERATIONS);
  printf("It will take roughly %lu iteration to do comb4\n", COMB4_ITERATIONS);

  int n = possibleWordsCount;
  int t;

  /* TODO: Run combR on multiple core */

  /* comb3 choose 3-words from n-words */
  /* The notation maps exactly to D. Knuth TAOCP V4A 7.2.1.3 (15) */
  t = 3;
  int c[t + 1];
  combR(t - 1, n - 1, c, t, words, knownAnagramHash);

  /* comb4 choose 3-words from n-words */
  /* The notation maps exactly to D. Knuth TAOCP V4A 7.2.1.3 (15) */
  t = 4;
  int d[t + 1];
  combR(t - 1, n - 1, d, t, words, knownAnagramHash);

  releaseWords(words);
  return 0;
}
