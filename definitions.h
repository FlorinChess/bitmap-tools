#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define INDEX_OFFSET 1
#define DEFAULT_STRING_SIZE 10
#define NULL_BYTE 1
#define BMP_HEADER_SIZE 54
#define BMP_PREFIX_SIZE 7

#include <stdlib.h>

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

typedef enum
{
  SUCCESS,
  CANNOT_EXECUTE_COMMAND,
  OUT_OF_MEMORY,
  INVALID_FILE,
  INVALID_USAGE,
  INVALID_OPTION,
  CANNOT_OPEN_FILE,
  MAX_LAYER_COLLECTION_CAPACITY_REACHED,
  BITMAP_FILE_CORRUPTED,
} ErrorCode;

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
char* getInput();
char** getUserInput();
size_t getLength(char** data);
bool isCharNumber(char character);
bool isStringumber(char* string);
bool isInitialUserInputValid(char** user_input);
void freeUserInput(char** user_input);

///
/// This function frees a char* array.
///
/// @param pointer_array The char* array.
///
void freePointerArray(char** pointer_array);

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

///
/// This function saves a layer to a .txt file.
///
/// @param file_path The name of the save file.
/// @param layer The layer to be saved.
///
ErrorCode saveLayerToFile(char* file_path, int layer_index);

#endif // DEFINITIONS_H
