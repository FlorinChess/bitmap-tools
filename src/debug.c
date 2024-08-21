#include "debug.h"

void printBinary(unsigned char byte) 
{
  for (int i = 7; i >= 0; i--) 
  {
    printf("%c", (byte & (1 << i)) ? '1' : '0');
  }
  printf("\n");
}

void printHexValues(const unsigned char *array, unsigned char length) 
{
  for (unsigned long i = 0; i < length; ++i)
  {
    printf("%02x", (unsigned char)array[i]);
    if (i < length - 1) 
    {
      printf(" ");
    }
  }
  printf("\n");
}
