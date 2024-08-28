#include "error.h"

void handleIfError(ErrorCode error_code)
{
  switch (error_code)
  {
  case SUCCESS:
    return;
    break;
  case INVALID_FILE:
    printf("Invalid file(s)!\n\n");
    break;
  case INVALID_USAGE:
    printf("Invalid usage!\n\n");
    break;
  case INVALID_OPTION:
    printf("Invalid option!\n\n");
    break;
  case INVALID_BMP_PREFIX:
    printf("Invalid encoding!\n\n");
    break;
  case CANNOT_OPEN_FILE:
    printf("Cannot open file!\n\n");
    break;
  case CANNOT_READ_FILE:
    printf("Cannot read from file!\n\n");
    break;
  case CANNOT_WRITE_FILE:
    printf("Cannot write to file!\n\n");
    break;
  case OUT_OF_MEMORY:
    printf("Program ran out of memory!\n\n");
    break;
  case MAX_LAYER_COLLECTION_CAPACITY_REACHED:
    printf("Cannot open more than 10 files!\n\n");
    break;
  default:
    printf("Unknown error occured!\n\n");
    break;
  }
  exit(error_code);
}
