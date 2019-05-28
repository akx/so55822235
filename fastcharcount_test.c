#include <stdio.h>
#include "fastcharcount.h"

int main(int argc, char **argv) {
  const char* test_string = "oispa kaljaa, mut noh, kaljaahan on";
  const char* haystack = "os ka";
  int n = xchar_count(test_string, strlen(test_string), haystack);
  printf("%s; %s; %d\n", test_string, haystack, n);
}