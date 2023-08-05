#pragma once

#include <stdint.h>

void *memset(void *b, int c, unsigned long len);
void *memcpy(void *dst, const void *src, unsigned long len);
void *memmove(void *dst, const void *src, unsigned long len);
int memcmp(const void *s1, const void *s2, unsigned long n);

unsigned long strlen(const char *str);

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, unsigned long n);
