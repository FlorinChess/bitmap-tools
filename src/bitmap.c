#include "definitions.h"
#include "bitmap.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

bool isBitmapPrefixValid(char bitmap_prefix[BMP_PREFIX_SIZE])
{
  printf("Checking encoding...\n");

  if (strncmp(bitmap_prefix, "BMP", 3) != 0)
    return false;

  for (size_t i = 3; i < BMP_PREFIX_SIZE; ++i)
    if (!isCharNumber(bitmap_prefix[i]))
      return false;

  return true;
}

ErrorCode readBitmapPixels(FILE* file, BitmapFileHeader file_header, BitmapInfoHeader info_header, unsigned char** pixel_buffer)
{
  int padding = info_header.bitmap_width_ % 4;
  size_t bytes_per_row = info_header.bitmap_width_ * 3;

  fseek(file, file_header.pixel_array_offset_, SEEK_SET);

  *pixel_buffer = malloc((bytes_per_row + padding) * info_header.bitmap_height_);

  if (*pixel_buffer == NULL)
    return OUT_OF_MEMORY;

  unsigned int buffer_index = 0;
  for (int row = 0; row < info_header.bitmap_height_; row++)
  {
    if (fread(pixel_buffer + buffer_index, sizeof(char), bytes_per_row, file) != bytes_per_row)
    {
      return INVALID_FILE;
    }
    
    buffer_index += bytes_per_row;

    fseek(file, padding, SEEK_CUR);
  }

  return SUCCESS;
}

ErrorCode readBitmapHeaders(FILE* file, BitmapFileHeader* file_header, BitmapInfoHeader* info_header)
{
  if (fread(file_header, sizeof(char), sizeof(BitmapFileHeader), file) != sizeof(BitmapFileHeader))
    return CANNOT_READ_FILE;
    
  if (fread(info_header, sizeof(char), sizeof(BitmapInfoHeader), file) != sizeof(BitmapInfoHeader))
    return CANNOT_READ_FILE;

  if (strncmp(file_header->magic_word_, BMP_MAGIC_WORD, 2) != 0)
    return INVALID_FILE;

  return SUCCESS;
}

ErrorCode decode(char* file_path)
{
  FILE* file = fopen(file_path, "rb");

  if (file == NULL)
    return CANNOT_OPEN_FILE;

  BitmapFileHeader file_header;
  BitmapInfoHeader info_header;
  
  ErrorCode error_code = readBitmapHeaders(file, &file_header, &info_header);

  if (error_code != SUCCESS)
  {
    fclose(file);
    return error_code;
  }

  int padding = info_header.bitmap_width_ % 4;
  size_t bytes_per_row = info_header.bitmap_width_ * 3;

  fseek(file, file_header.pixel_array_offset_, SEEK_SET);

  unsigned char* pixel_buffer = malloc((bytes_per_row + padding) * info_header.bitmap_height_);
  unsigned int buffer_index = 0;
  for (int row = 0; row < info_header.bitmap_height_; row++)
  {
    if (fread(pixel_buffer + buffer_index, sizeof(char), bytes_per_row, file) != bytes_per_row)
    {
      fclose(file);
      free(pixel_buffer);
      return INVALID_FILE;
    }
    
    buffer_index += bytes_per_row;

    fseek(file, padding, SEEK_CUR);
  }

  buffer_index = 0;
  char bmp_prefix_buffer[BMP_PREFIX_SIZE] = {0};

  for (size_t i = 0; i < BMP_PREFIX_SIZE; i++)
  {
    for (size_t j = 0; j < 8; j++)
    {
      char bit_to_decode = pixel_buffer[buffer_index++] & 0x01;
      bmp_prefix_buffer[i] = (bmp_prefix_buffer[i] << 1) | bit_to_decode;
    }
  }

  if (!isBitmapPrefixValid(bmp_prefix_buffer))
  {
    free(pixel_buffer);
    fclose(file);
    return INVALID_BMP_PREFIX;
  }

  printf("Valid encoding\n");

  char length_buffer[5] = {0}; 
  memcpy(length_buffer, bmp_prefix_buffer + 3, 4);

  size_t length = atoi(length_buffer);

  char message_buffer[length + NULL_BYTE];
  memset(message_buffer, 0, sizeof(message_buffer));

  for (size_t i = 0; i < length; i++)
  {
    for (size_t j = 0; j < 8; j++)
    {
      char bit_to_decode = pixel_buffer[buffer_index++] & 0x01;

      message_buffer[i] = (message_buffer[i] << 1) | bit_to_decode;
    }
  }

  message_buffer[length] = '\0';

  printf(
    "--------------------------------------------\n"
    "Message length:  %ld characters\n" 
    "Message content: %s\n"
    "--------------------------------------------\n",
    length, message_buffer);

  free(pixel_buffer);
  fclose(file);

  return SUCCESS;
}

ErrorCode encode(char* file_path, char* message)
{
  // Subject to change
  char* output_file = "output.bmp"; 

  cp(output_file, file_path);

  FILE* file = fopen(output_file, "rb+");

  if (file == NULL)
    return CANNOT_OPEN_FILE;

  BitmapFileHeader file_header;
  BitmapInfoHeader info_header;
  
  ErrorCode error_code = readBitmapHeaders(file, &file_header, &info_header);

  if (error_code != SUCCESS)
  {
    fclose(file);
    return error_code;
  }

  int padding = info_header.bitmap_width_ % 4;
  size_t bytes_per_row = info_header.bitmap_width_ * 3;

  fseek(file, file_header.pixel_array_offset_, SEEK_SET);

  unsigned char* pixel_buffer = malloc((bytes_per_row + padding) * info_header.bitmap_height_);
  unsigned int buffer_index = 0;
  for (int row = 0; row < info_header.bitmap_height_; row++)
  {
    if (fread(pixel_buffer + buffer_index, sizeof(char), bytes_per_row, file) != bytes_per_row)
    {
      fclose(file);
      free(pixel_buffer);
      return INVALID_FILE;
    }
    
    buffer_index += bytes_per_row;

    fseek(file, padding, SEEK_CUR);
  }

  size_t message_length = strlen(message);
  
  char message_length_string[4];
  lengthToString(message_length, message_length_string);

  char* message_to_encode = malloc(BMP_PREFIX_SIZE + message_length);

  memcpy(message_to_encode, "BMP", 3);
  memcpy(message_to_encode + 3, message_length_string, 4);
  memcpy(message_to_encode + 7, message, message_length);

  size_t char_index = 0;
  ssize_t bit_index = 7;
  size_t wbuffer_index = 0;

  // Add message to the byte array
  while(char_index < BMP_PREFIX_SIZE + message_length)
  {
    char bit_to_encode = (message_to_encode[char_index] >> bit_index--) & 0x01;

    if (bit_index < 0)
    {
      bit_index = 7;
      char_index++;
    }

    // clear LSB and store bit to encode into it
    pixel_buffer[wbuffer_index] &= ~0x01;
    pixel_buffer[wbuffer_index] |= bit_to_encode;

    wbuffer_index++;
  }

  size_t bytes_to_encode = wbuffer_index;
  wbuffer_index = 0;

  // Rewind to pixel array start and rewrite modified bytes
  fseek(file, file_header.pixel_array_offset_, SEEK_SET);
  for (int row = 0; row < info_header.bitmap_height_; row++)
  {
    for (int column = 0; column < info_header.bitmap_width_; column++)
    {
      if (wbuffer_index >= bytes_to_encode)
        goto writting_finished;

      if (fwrite(pixel_buffer + wbuffer_index, sizeof(char), 3, file) != 3)
      {
        free(pixel_buffer);
        free(message_to_encode);
        fclose(file);
        return CANNOT_WRITE_FILE;
      }
      
      wbuffer_index += 3;
    }

    fseek(file, padding, SEEK_CUR);
  }

  writting_finished:

  printf("Message successfully encoded!\n");

  free(pixel_buffer);
  free(message_to_encode);
  fclose(file);

  return SUCCESS;
}
