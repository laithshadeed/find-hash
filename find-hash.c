/* compile with:
 * clang -std=c89 -lcrypto find-hash.c -o find-hash
 * Run with:
 * ./find-hash wordlist
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

#define HASH_SIZE 26
#define MAX_WORD_SIZE 25
#define MAX_ANAGRAM_SIZE 21
#define MAX_NUM_WORDS 1659

int getHash(const char * word, int * const hash) {
  int i;
  for (i = 0; i < strlen(word); i += 1) {
    char c = word[i];
    int index = c - 97;
    if (index < 0 || index > 25) return 0;
    hash[index] = hash[index] != 0 ? hash[index] + 1 : 1;
  }
  return 1;
}

int isValidHash(const char * word, const int * knownHash) {
  int hash[HASH_SIZE] = {0};
  int canHash = getHash(word, hash);
  int i = 0;
  for (i = 0; i < HASH_SIZE; i += 1) {
    if (canHash == 0 || hash[i] > knownHash[i]) {
      return 0;
    }
  }

  return 1;
}

int isKnownWord(char * words[], const char * newWord) {
  int i = 0;
  for (i = 0; i < MAX_NUM_WORDS; i += 1) {
    if (strcmp(words[i], newWord) == 0) {
      return 1;
    }
  }
  return 0;
}

void getPossibleWord(const char * filepath, int * knownAnagramHash, char * words[]) {
  FILE * inputFd;
  char line[MAX_WORD_SIZE];
  int wordIndex = -1;
  inputFd = fopen(filepath, "r");
  if (inputFd == NULL) {
    printf("opening file %s failed\n", filepath);
    return;
  }

  while(fgets(line, MAX_WORD_SIZE, inputFd) != NULL) {
    /* Strip LF & CRLF */
    line[strcspn(line, "\r\n")] = '\0';
    if (*line == '\0') continue;

    int isMyHashValid = isValidHash(line, knownAnagramHash);

    if (isMyHashValid == 0) continue;

    int isMyWordKnown = isKnownWord(words, line);

    if (isMyWordKnown == 1) continue;

    wordIndex +=1;
    strcpy(words[wordIndex], line);
  }

  if (fclose(inputFd) == -1) {
    printf("close input file %s failed\n", filepath);
  }
}

int isAnagram(const char word[], int wordLength, const int * knownHash) {
  int hash[HASH_SIZE] = {0};
  int i = 0;
  for (i = 0; i < wordLength; i += 1) {
    char c = word[i];
    if (c == ' ') continue; /* Skip space character */
    int index = c - 97;
    if (index < 0 || index > 25) return 0; /* Fail if outsize a-z range */
    hash[index] = hash[index] != 0 ? hash[index] + 1 : 1;
  }

  for (i = 0; i < HASH_SIZE; i += 1) {
    if (hash[i] != knownHash[i]) {
      return 0;
    }
  }

  return 1;
}

void md5sum(char phrase[], int currentLength, char md5[]) {
		int i;
		unsigned char md5sum[MD5_DIGEST_LENGTH];
		MD5((unsigned char *)phrase, currentLength - 1, md5sum);
		for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
      sprintf(&md5[i*2], "%02x", (unsigned int)md5sum[i]);
		}
}

int isValidMD5(char s1[], char s2[]) {
  int i = 0;
  for (i = 0; i < 32; i += 1) {
    if (s1[i] != s2[i]) return 0;
  }

  return 1;
}

void findHash(char * words[], char phrase[], int currentLength, int maxNumberOfWords,
    const int * knownAnagramHash) {
  /* phrase always has space at the end so currentLength should be minus 1 */
  if (maxNumberOfWords > 4) return;
  if (currentLength - 1 == MAX_ANAGRAM_SIZE || currentLength == MAX_ANAGRAM_SIZE) {
    if (isAnagram(phrase, currentLength, knownAnagramHash) == 0) return;
    char md5[33];
    md5sum(phrase, currentLength, md5);

    if ( isValidMD5(md5, "e4820b45d2277f3844eac66c903e84be") == 0 &&
         isValidMD5(md5, "23170acc097c24edb98fc5488ab033fe") == 0 &&
         isValidMD5(md5, "665e5bcb0c20062fe8abaaf4628bb154") == 0 ) return;

		int m = 0;
		for (m = 0; m < currentLength - 1; m += 1) {
			printf("%c", phrase[m]);
		}
		printf("\n");
  } else {
    int i;
    for (i = 0; i < MAX_NUM_WORDS; i += 1) {
      int wordLen = strlen(words[i]);
      int newLength = currentLength;
      newLength += wordLen + 1; /* extra one for space */
      if (newLength - 1 > MAX_ANAGRAM_SIZE) continue;

      int m = 0;
      for (m = 0; m < wordLen; m += 1) {
        phrase[m + currentLength] = words[i][m];
      }
      phrase[newLength - 1] = ' ';

      findHash(words, phrase, newLength, maxNumberOfWords + 1, knownAnagramHash);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    printf("Usage: %s worldlist\n", argv[0]);
    return 1;
  }

  const char * filepath = argv[1];
  const char * knownAnagramNoSpaces = "poultryoutwitsants";
  /*const char * knownAnagram = "poultry outwits ants";*/

  int knownAnagramHash[HASH_SIZE] = {0};
  getHash(knownAnagramNoSpaces, knownAnagramHash);

  char * words[MAX_NUM_WORDS];
  int i = 0;
  for (i = 0; i < MAX_NUM_WORDS; i += 1) {
    words[i] = malloc(MAX_WORD_SIZE * sizeof(char));
    strcpy(words[i], "\0");
  }

  getPossibleWord(filepath, knownAnagramHash, words);

  const int maxPhraseSize =  MAX_ANAGRAM_SIZE + 2; /* extra 2 for space & \0 */
  char phrase[maxPhraseSize];
  int currentLength = 0;
  int maxNumberOfWords = 0;
  findHash(words, phrase, currentLength, maxNumberOfWords, knownAnagramHash);

  for (i = 0; i < MAX_NUM_WORDS; i += 1) {
    free(words[i]);
  }

  return 0;
}
