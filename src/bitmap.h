#ifndef BITMAP_H
#define BITMAP_H

#include "error.h"
#include "layer.h"
#include <string.h>
#include <sys/types.h>

#define BMP_PREFIX_SIZE 7
#define BMP_MAGIC_WORD "BM"

typedef struct _BitmapFileHeader_
{
  char magic_word_[2];                   // 0x00
  unsigned int file_size_;               // 0x02
  unsigned short reserved_1_;            // 0x06
  unsigned short reserved_2_;            // 0x08
  int pixel_array_offset_;               // 0x0A
} __attribute__((__packed__)) BitmapFileHeader;

typedef struct _BitmapInfoHeader_
{
  unsigned int info_header_size_;        // 0x0E
  int bitmap_width_;                     // 0x12
  int bitmap_height_;                    // 0x16
  unsigned short color_planes_;          // 0x1A
  unsigned short bits_per_pixel_;        // 0x1C
  unsigned int compression_method_;      // 0x1E
  unsigned int raw_image_size_;          // 0x22
  int horizontoal_pixels_per_meter_;     // 0x26
  int vertical_pixels_per_meter_;        // 0x2A
  unsigned int number_of_colors_;        // 0x2E
  unsigned int number_important_colors_; // 0x32
} __attribute__((__packed__)) BitmapInfoHeader;

bool isBitmapPrefixValid(char bitmap_prefix[BMP_PREFIX_SIZE]);

ErrorCode readBitmapHeaders(FILE* file, BitmapFileHeader* file_header, BitmapInfoHeader* info_header);

ErrorCode decode(char* file_path);

ErrorCode encode(char* file_path, char* message);

#endif // BITMAP_H
