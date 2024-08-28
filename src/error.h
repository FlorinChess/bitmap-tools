#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>
#include <stdio.h>

#define NULL_BYTE 1

typedef enum _ErrorCode_
{
  SUCCESS,
  CANNOT_EXECUTE_COMMAND,
  INVALID_FILE,
  INVALID_USAGE,
  INVALID_OPTION,
  INVALID_BMP_PREFIX,
  CANNOT_OPEN_FILE,
  CANNOT_READ_FILE,
  CANNOT_WRITE_FILE,
  OUT_OF_MEMORY,
  MAX_LAYER_COLLECTION_CAPACITY_REACHED
} ErrorCode;

void handleIfError(ErrorCode error_code);

#endif // ERROR_H
