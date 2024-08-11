#ifndef BITMAP_H
#define BITMAP_H

typedef struct
{
  unsigned short magic_word_;            // 0x00
  unsigned int file_size_;               // 0x02
  unsigned short reserved_1_;            // 0x06
  unsigned short reserved_2_;            // 0x08
  int pixel_array_offset_;               // 0x0A
} BitmapFileHeader;

typedef struct {
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
} BitmapInfoHeader;

// typedef struct {
//   DWORD        bV5Size;
//   LONG         bV5Width;
//   LONG         bV5Height;
//   WORD         bV5Planes;
//   WORD         bV5BitCount;
//   DWORD        bV5Compression;
//   DWORD        bV5SizeImage;
//   LONG         bV5XPelsPerMeter;
//   LONG         bV5YPelsPerMeter;
//   DWORD        bV5ClrUsed;
//   DWORD        bV5ClrImportant;
//   DWORD        bV5RedMask;
//   DWORD        bV5GreenMask;
//   DWORD        bV5BlueMask;
//   DWORD        bV5AlphaMask;
//   DWORD        bV5CSType;
//   CIEXYZTRIPLE bV5Endpoints;
//   DWORD        bV5GammaRed;
//   DWORD        bV5GammaGreen;
//   DWORD        bV5GammaBlue;
//   DWORD        bV5Intent;
//   DWORD        bV5ProfileData;
//   DWORD        bV5ProfileSize;
//   DWORD        bV5Reserved;
// } BITMAPV5HEADER, *LPBITMAPV5HEADER, *PBITMAPV5HEADER;



#endif // BITMAP_H
