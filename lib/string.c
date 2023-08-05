#include "string.h"
#include <stdint.h>

void *memset(void *b, int c, unsigned long len) {
  uint8_t *p = b;
  while (len--) {
    *p++ = (uint8_t)c;
  }
  return b;
}

void *memcpy(void *dst, const void *src, unsigned long len) {
  uint8_t *d = dst;
  const uint8_t *s = src;
  while (len--) {
    *d++ = *s++;
  }
  return dst;
}

void *memmove(void *dst, const void *src, unsigned long len) {
  uint8_t *d = dst;
  const uint8_t *s = src;
  if (d < s) {
    while (len--) {
      *d++ = *s++;
    }
  } else {
    d += len;
    s += len;
    while (len--) {
      *--d = *--s;
    }
  }
  return dst;
}

int memcmp(const void *b1, const void *b2, unsigned long len) {
  const uint8_t *p1 = b1, *p2 = b2;
  while (len--) {
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    p1++;
    p2++;
  }
  return 0;
}

unsigned long strlen(const char *str) {
  unsigned long len = 0;
  while (*str++) {
    len++;
  }
  return len;
}

int strcmp(const char *s1, const char *s2) {
  if (strlen(s1) != strlen(s2)) {
    return strlen(s1) - strlen(s2);
  }
  return memcmp(s1, s2, strlen(s1));
}

int strncmp(const char *s1, const char *s2, unsigned long n) {
  return memcmp(s1, s2, n);
}
