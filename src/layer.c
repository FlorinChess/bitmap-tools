#include "layer.h"
#include <string.h>

extern LayerCollection layer_collection_;

ErrorCode addLayer(Layer* new_layer)
{
  if (layer_collection_.count_ == 10)
    return MAX_LAYER_COLLECTION_CAPACITY_REACHED;

  layer_collection_.layers_[layer_collection_.count_] = new_layer;
  layer_collection_.count_++;

  return SUCCESS;
}

Layer* createLayer(size_t width, size_t height)
{
  Layer* new_layer = malloc(sizeof(Layer));

  if (new_layer == NULL)
    return NULL;

  char** pixels = createLayerPixels(width, height);

  if (pixels == NULL)
  {
    free(new_layer);
    return NULL;
  }

  new_layer->width_ = width;
  new_layer->height_ = height;
  new_layer->pixels_ = pixels;

  return new_layer;
}

char** createLayerPixels(size_t width, size_t height)
{
  char** pixels = calloc(height, sizeof(char*));

  if (pixels == NULL)
    return NULL;

  for (size_t i = 0; i < height; ++i)
  {
    pixels[i] = malloc(width * sizeof(char) + NULL_BYTE);

    if (pixels[i] == NULL)
    {
      free(pixels);
      return NULL;
    }

    size_t j = 0;

    for (j = 0; j < width; j++)
    {
      pixels[i][j] = ' ';
    }
    pixels[i][j] = '\0';
  }

  return pixels;
}

ErrorCode saveLayerToFile(char* file_path, int layer_index)
{
  size_t length = strlen(file_path);

  char* new_filename = malloc(length + 5); // +4 ".txt", +1 null terminator

  if (new_filename == NULL)
  {
    return OUT_OF_MEMORY;
  }

  strcpy(new_filename, file_path);
  strcpy(new_filename + length, ".txt");

  FILE* save_file = fopen(new_filename,"w");

  if (save_file == NULL)
  {
    free(new_filename);
    return CANNOT_OPEN_FILE;
  }


  for (size_t row = 0; row < layer_collection_.layers_[layer_index]->height_; row++)
  {
    fprintf(save_file, "%s", layer_collection_.layers_[layer_index]->pixels_[row]);
    fprintf(save_file, "\n");
  }

  fclose(save_file);

  printf("%s.bmp converted to ASCII art -> %s\n", file_path, new_filename);
  free(new_filename);

  return SUCCESS;
}

void freeLayer(Layer* layer)
{
  for (size_t row = 0; row < layer->height_; row++)
  {
    free(layer->pixels_[row]);
  }

  free(layer->pixels_);
  free(layer);
}

void freeLayerCollection()
{
  for (size_t layer_i = 0; layer_i < layer_collection_.count_; layer_i++)
  {
    freeLayer(layer_collection_.layers_[layer_i]);
  }
}

int convertPixelToGrayscale(Pixel* pixel)
{
  return pixel->red_ * 0.2126 + pixel->green_ * 0.7152 + pixel->blue_ * 0.0722;
}

char convertGrayscaleToSymbol(int grayscale_value)
{
  if (0 <= grayscale_value && grayscale_value <= 19)
    return '@';
  else if (20 <= grayscale_value && grayscale_value <= 39)
    return '$';
  else if (40 <= grayscale_value && grayscale_value <= 59)
    return '#';
  else if (60 <= grayscale_value && grayscale_value <= 79)
    return '*';
  else if (80 <= grayscale_value && grayscale_value <= 99)
    return '!';
  else if (100 <= grayscale_value && grayscale_value <= 119)
    return '=';
  else if (120 <= grayscale_value && grayscale_value <= 139)
    return ';';
  else if (140 <= grayscale_value && grayscale_value <= 159)
    return ':';
  else if (160 <= grayscale_value && grayscale_value <= 179)
    return '~';
  else if (180 <= grayscale_value && grayscale_value <= 199)
    return '-';
  else if (200 <= grayscale_value && grayscale_value <= 219)
    return ',';
  else if (220 <= grayscale_value && grayscale_value <= 239)
    return '.';
  else if (240 <= grayscale_value && grayscale_value <= 255)
    return ' ';

  return 'x';
}
