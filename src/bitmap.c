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

ErrorCode encode(char* file_path, char* message)
{
  // Subject to change
  char* output_file = "output.bmp"; 

  cp(output_file, file_path);

  FILE* file = fopen(output_file, "rb+");

  if (file == NULL)
    return CANNOT_OPEN_FILE;

  uint8_t header[BMP_HEADER_SIZE];
  size_t fread_return = fread(header, sizeof(uint8_t), BMP_HEADER_SIZE, file);

  if (fread_return != sizeof(header))
  {
    fclose(file);
    return INVALID_FILE;
  }

  // Check magic number
  if (header[0] != 'B' || header[1] != 'M')
  {
    fclose(file);
    return BITMAP_FILE_CORRUPTED;
  }

  // int file_size =            *(int*)&header[0x02]; // 0x02 => 2
  int position_image_array = *(int*)&header[0x0A]; // 0x0a => 10
  // int info_header_size =     *(int*)&header[0x0E];
  int width_pixels =         *(int*)&header[0x12]; // 0x12 => 18
  int height_pixels =        *(int*)&header[0x16]; // 0x16 => 22
  // short bits_per_pixel =   *(short*)&header[0x1C]; // 0x1C => 28
  int padding = width_pixels % 4;

  // printf("width = %d, height = %d\n", width_pixels, height_pixels);
  // printf("BMP size (bytes) = %d info header size = %d, bits_per_pixel = %d\n", file_size, info_header_size, bits_per_pixel);

  fseek(file, position_image_array, SEEK_SET);

  unsigned char* malloc_buffer = malloc(20 * 1024 * 1024);
  int buffer_index = 0;
  for (int row = 0; row < height_pixels; row++)
  {
    fread_return = fread(malloc_buffer + buffer_index, sizeof(char), 3 * width_pixels, file);

    if (fread_return != (size_t)(3 * width_pixels))
    {
      fclose(file);
      return INVALID_FILE;
    }
    
    buffer_index += 3 * width_pixels;

    fseek(file, padding, SEEK_CUR);
  }

  size_t message_length = strlen(message);
  
  char bmp_prefix[] = "BMP";
  char message_length_string[4];
  lengthToString(message_length, message_length_string);

  char* message_to_encode = malloc(BMP_PREFIX_SIZE + message_length);

  memcpy(message_to_encode, bmp_prefix, 3);
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
    malloc_buffer[wbuffer_index] &= ~0x01;
    malloc_buffer[wbuffer_index] |= bit_to_encode;

    wbuffer_index++;
  }

  size_t bytes_to_encode = wbuffer_index;
  wbuffer_index = 0;

  // Rewind to pixel array start and rewrite modified bytes
  fseek(file, position_image_array, SEEK_SET);
  for (int row = 0; row < height_pixels; row++)
  {
    for (int column = 0; column < width_pixels; column++)
    {
      if (wbuffer_index >= bytes_to_encode)
        goto writting_finished;

      size_t return_value = fwrite(malloc_buffer + wbuffer_index, sizeof(char), 3, file);

      if (return_value != 3)
      {
        printf("Error occured when writting!! wbuffer_index = %lu\n", wbuffer_index);
        free(malloc_buffer);
        free(message_to_encode);
        fclose(file);
        exit(-1);
      }
      
      wbuffer_index += 3;
   }

   fseek(file, padding, SEEK_CUR);
  }

  writting_finished:

  free(malloc_buffer);
  free(message_to_encode);
  fclose(file);

  return SUCCESS;
}
