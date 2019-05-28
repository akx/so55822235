#include <unistd.h>

#ifndef FASTCHARCOUNT_QUALIFIER
#define FASTCHARCOUNT_QUALIFIER static
#endif

FASTCHARCOUNT_QUALIFIER size_t fastcharcount(const char *str, const char *haystack) {
  size_t count = 0;

  // Prepare lookup table.
  // It will contain 1 for all characters in the haystack.
  unsigned char table[256] = {0};
  unsigned char *ts = (unsigned char *)haystack;
  while(*ts) table[*ts++] = 1;

  unsigned char *s = (unsigned char *)str;
  #define CHECK_CHAR(i) { if(!s[i]) break; count += table[s[i]]; }
  for(;;) {
    CHECK_CHAR(0);
    CHECK_CHAR(1);
    CHECK_CHAR(2);
    CHECK_CHAR(3);
    s += 4;
  }
  #undef CHECK_CHAR
  return count;
}