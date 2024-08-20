#ifndef ERROR_H
#define ERROR_H

typedef enum
{
  SUCCESS,
  CANNOT_EXECUTE_COMMAND,
  OUT_OF_MEMORY,
  INVALID_FILE,
  INVALID_USAGE,
  INVALID_OPTION,
  INVALID_BMP_PREFIX,
  CANNOT_OPEN_FILE,
  MAX_LAYER_COLLECTION_CAPACITY_REACHED,
  BITMAP_FILE_CORRUPTED,
} ErrorCode;

#endif // ERROR_H
