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

/// @brief This function checks if the message encodeing of a .bmp file has the valid prefix
/// a length between 0 and 9999 characters.
///
/// @param bitmap_prefix The buffer containing the encoding prefix as a string.
///
/// @return False if the prefix does not match the expected value as defined by BMP_PREFIX or if the length is invalid, else true. 
///
bool isBitmapPrefixValid(const char bitmap_prefix[BMP_PREFIX_SIZE]);

/// @brief This function reads the headers of a bitmap file. 
/// @param file The file to read from.
/// @param file_header The buffer for the file headers of the bitmap file.
/// @param info_header The buffer for the info headers of the bitmap file.
/// @return 
/// INVALID_FILE if the parameter file does point to a bitmap file containing the correct magic word as defined by BMP_MAGIC_WORD,
/// CANNOT_READ_FILE if the parameter file does point to a bitmap that cannot be read,
/// else SUCCESS.
ErrorCode readBitmapHeaders(FILE* file, BitmapFileHeader* file_header, BitmapInfoHeader* info_header);

/// @brief This message decodes a message from a .bmp file.
/// @param file_path The full file path of the .bmp file. 
/// @param message The message to be encoded.
/// @return 
/// CANNOT_OPEN_FILE if the file at the given file path cannot be opened,
/// CANNOT_READ_FILE if the file at the given file path cannot be read,
/// CANNOT_WRITE_FILE if the file at the given file path cannot be written to,
/// INVALID_FILE if the file at the given file path does not contain the correct magic word as defined by BMP_MAGIC_WORD,
/// INVALID_BMP_PREFIX if the file at the given file path does not contain a valid bitmap encoding,
/// OUT_OF_MEMORY if the program cannot allocate any more memory,
/// else SUCCESS.
ErrorCode decode(const char* file_path);

/// @brief This message encodes a message into a .bmp file.
/// @param file_path The full file path of the .bmp file. 
/// @param message The message to be encoded.
/// @return 
/// CANNOT_OPEN_FILE if the file at the given file path cannot be opened,
/// CANNOT_READ_FILE if the file at the given file path cannot be read,
/// INVALID_FILE if the file at the given file path does not contain the correct magic word as defined by BMP_MAGIC_WORD,
/// OUT_OF_MEMORY if the program cannot allocate any more memory,
/// else SUCCESS.
ErrorCode encode(const char* file_path, const char* message);

///
/// @brief This function reads a .bmp file and creates a layer of all the pixels in it converted to ASCII characters.
/// The new layer is added to the layer collection.
///
/// @param file The .bmp file.
/// @param layer_collection The layer collection
///
/// @return
/// CANNOT_OPEN_FILE if the file at the given file path cannot be opened,
/// CANNOT_READ_FILE if the file at the given file path cannot be read,
/// INVALID_FILE if the file at the given file path does not contain the correct magic word as defined by BMP_MAGIC_WORD,
/// OUT_OF_MEMORY if the program cannot allocate any more memory,
/// else SUCCESS.
ErrorCode convertBitmapToASCII(const char* file_path);

#endif // BITMAP_H
