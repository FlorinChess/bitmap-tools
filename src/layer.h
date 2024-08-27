#ifndef LAYER_H
#define LAYER_H

#include "error.h"

typedef struct _Pixel_
{
  unsigned char blue_;
  unsigned char green_;
  unsigned char red_;
} __attribute__((__packed__)) Pixel;

typedef struct _Layer_
{
  size_t width_;
  size_t height_;
  char** pixels_;
} Layer;

typedef struct _LayerCollection_
{
  unsigned count_;
  Layer* layers_[10];
} LayerCollection;

///
/// @brief This function creates a layer and allocates it on the heap.
///
/// @param width The width of the new layer.
/// @param height The height of the new layer.
/// @param x The x coordinate of the new layer.
/// @param y The y cooridnate of the new layer.
///
/// @returns The new layer.
Layer* createLayer(size_t width, size_t height);

///
/// @brief This function creates a char matrix and allocates it on the heap.
///
/// @param width The width of the matrix.
/// @param height The height of the matrix.
///
/// @returns The new matrix.
char** createLayerPixels(size_t width, size_t height);

///
/// @brief This function adds a new layer to the layer collection.
///
/// @param layer_collection The layer collection.
/// @param new_layer The new layer to be added.
///
/// @returns The error code:
///   OUT_OF_MEMROY
///   SUCCESS
ErrorCode addLayer(Layer* new_layer);

///
/// @brief This function saves a layer to a .txt file.
///
/// @param file_path The name of the save file.
/// @param layer The layer to be saved.
///
ErrorCode saveLayerToFile(char* file_path, int layer_index);

///
/// @brief This function frees a given layer.
///
/// @param layer The layer to be freed.
///
void freeLayer(Layer* layer);

///
/// @brief This function frees the layer collection.
///
/// @param layer_collection The layer collection.
///
void freeLayerCollection();

///
/// @brief This function converts the RGB values of a pixel to grayscale.
///
/// @param pixel The pixel.
///
/// @returns The grayscale value.
///
int convertPixelToGrayscale(Pixel* pixel);

///
/// @brief This function converts the grayscale value to the coresponding ASCII symbol.
///
/// @param grayscale_value The grayscale value.
///
/// @returns The ASCII symbol.
///
char convertGrayscaleToSymbol(int grayscale_value);

#endif // LAYER_H
