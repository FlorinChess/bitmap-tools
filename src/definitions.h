#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define INDEX_OFFSET 1
#define DEFAULT_STRING_SIZE 10
#define NULL_BYTE 1
#define BMP_HEADER_SIZE 54
#define BMP_PREFIX_SIZE 7

#include <stdbool.h>
#include <stddef.h>
#include "error.h"

typedef struct
{
  int red_;
  int green_;
  int blue_;
} Pixel;

typedef struct
{
  size_t width_;
  size_t height_;
  char** pixels_;
} Layer;

typedef struct
{
  unsigned count_;
  Layer* layers_[10];
} LayerCollection;

void handleIfError(ErrorCode error_code);

///
/// This function creates a layer and allocates it on the heap.
///
/// @param width The width of the new layer.
/// @param height The height of the new layer.
/// @param x The x coordinate of the new layer.
/// @param y The y cooridnate of the new layer.
///
/// @returns The new layer.
Layer* createLayer(size_t width, size_t height);

///
/// This function creates a char matrix and allocates it on the heap.
///
/// @param width The width of the matrix.
/// @param height The height of the matrix.
///
/// @returns The new matrix.
char** createLayerPixels(size_t width, size_t height);

///
/// This function retrievs input from the user as a char* and allocates it on the heap.
///
/// @returns The user input.
//
char* getInput();

void lengthToString(size_t length, char buffer[4]);

///
/// This function reads a .bmp file and creates a layer of all the pixels in it converted to ASCII characters.
/// The new layer is added to the layer collection.
///
/// @param file The .bmp file.
/// @param layer_collection The layer collection
///
/// @returns The error code:
///   OUT_OF_MEMROY
///   SUCCESS
ErrorCode parseBinaryFile(char* file_path);

///
/// This function saves a layer to a .txt file.
///
/// @param file_path The name of the save file.
/// @param layer The layer to be saved.
///
ErrorCode saveLayerToFile(char* file_path, int layer_index);

///
/// This function adds a new layer to the layer collection.
///
/// @param layer_collection The layer collection.
/// @param new_layer The new layer to be added.
///
/// @returns The error code:
///   OUT_OF_MEMROY
///   SUCCESS
ErrorCode addLayer(Layer* new_layer);

///
/// This function frees a given layer.
///
/// @param layer The layer to be freed.
///
void freeLayer(Layer* layer);

///
/// This function frees the layer collection.
///
/// @param layer_collection The layer collection.
///
void freeLayerCollection();

///
/// This function converts the rgb values of a pixel to grayscale.
///
/// @param pixel The pixel.
///
/// @returns The grayscale value.
int pixelRGBToGrayscaleValue(Pixel* pixel);

///
/// This function converts the grayscale value to the coresponding ASCII symbol.
///
/// @param grayscale_value The grayscale value.
///
/// @returns The ASCII symbol.
///
char convertGrayscaleToSymbol(int grayscale_value);

char* getFilePathWithoutExtension(char* file_path);

void printBinary(unsigned char byte);

bool isCharNumber(char character);

bool isBitmapPrefixValid(char bitmap_prefix[BMP_PREFIX_SIZE]);

int cp(const char* to, const char* from);

ErrorCode decode(char* file_path);

ErrorCode encode(char* file_path, char* message);

void printHelpMessage();

void handleIfError(ErrorCode error_code);

ErrorCode checkParameters(char* parameters[], const size_t const);

bool isUserInputValid(char* user_input);

#endif // DEFINITIONS_H
