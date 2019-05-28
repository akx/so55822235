/* Copyright (C) 1991-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

// adapted from
// https://github.com/bminor/glibc/blob/fe92a91f1e713ba4951bc6e6162f8803dc76b816/string/strpbrk.c

#include "libc-pointer-arith.h"
#include <string.h>

/* Return the length of the maximum initial segment of S which contains no
 * characters from REJECT.  */
static size_t glibc_strcspn(const char *str, const char *reject) {
  // if ( (reject[0] == '\0') || (reject[1] == '\0'))
  //  return __strchrnul (str, reject [0]) - str;

  /* Use multiple small memsets to enable inlining on most targets.  */
  unsigned char table[256];
  unsigned char *p = memset(table, 0, 64);
  memset(p + 64, 0, 64);
  memset(p + 128, 0, 64);
  memset(p + 192, 0, 64);

  unsigned char *s = (unsigned char *)reject;
  unsigned char tmp;
  do
    p[tmp = *s++] = 1;
  while (tmp);

  s = (unsigned char *)str;
  if (p[s[0]])
    return 0;
  if (p[s[1]])
    return 1;
  if (p[s[2]])
    return 2;
  if (p[s[3]])
    return 3;

  s = (unsigned char *)PTR_ALIGN_DOWN(s, 4);

  unsigned int c0, c1, c2, c3;
  do {
    s += 4;
    c0 = p[s[0]];
    c1 = p[s[1]];
    c2 = p[s[2]];
    c3 = p[s[3]];
  } while ((c0 | c1 | c2 | c3) == 0);

  size_t count = s - (unsigned char *)str;
  return (c0 | c1) != 0 ? count - c0 + 1 : count - c2 + 3;
}

/* Find the first occurrence in S of any character in ACCEPT.  */
static char *glibc_strpbrk(const char *s, const char *accept) {
  s += glibc_strcspn(s, accept);
  return *s ? (char *)s : NULL;
}