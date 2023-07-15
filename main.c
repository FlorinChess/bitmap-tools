#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#define INDEX_OFFSET 1
#define DEFAULT_STRING_SIZE 10
#define EDGES 2
#define NULL_BYTE 1
#define RESET_SEQUENCE "\033[0m"

typedef struct _Pixel_
{
  int red_;
  int green_;
  int blue_;
} Pixel;

typedef struct _Layer_
{
  size_t x_start_;
  size_t y_start_;
  size_t width_;
  size_t height_;
  char** pixels_;
  int rotation_;
  char* color_end_;
} Layer;

typedef struct _LayerCollection_
{
  unsigned count_;
  Layer** layers_;
} LayerCollection;

typedef enum _CommandCode_
{
  LOAD,
  RESIZE,
  DRAW,
  FILL,
  ROTATE,
  UNDO,
  SAVE,
  COLOR,
  QUIT,
  INVALID_COMMAND
} CommandCode;

typedef enum _ErrorCode_
{
  SUCCESS,
  CANNOT_PARSE_COORDINATES,
  CANNOT_EXECUTE_COMMAND,
  OUT_OF_MEMORY,
  FILE_INVALID
} ErrorCode;

void printHorizontalLines(size_t width);
void printLayer(Layer* layer);
Layer* createLayer(size_t width, size_t height, size_t x, size_t y);
char** createLayerPixels(size_t width, size_t height);
char* getInput();
char** getUserInput();
size_t getLength(char** data);
bool isCharNumber(char character);
bool isStringumber(char* string);
bool isInitialUserInputValid(char** user_input);
void freeUserInput(char** user_input);
void freePointerArray(char** pointer_array);
ErrorCode parseCoordinates(size_t* x, size_t* y, char* coordinates);
ErrorCode load(char** parameters, LayerCollection* layer_collection);
ErrorCode resize(char** parameters, LayerCollection* layer_collection);
ErrorCode fill(char** parameters, LayerCollection* layer_collection, bool* undo_available);
void transposeMatrix(Layer* layer);
void switchRows(Layer* layer);
void rotateLayer(Layer* layer);
void rotate(LayerCollection* layer_collection);
void undo(LayerCollection* layer_collection);
ErrorCode save(char** parameters, LayerCollection* layer_collection);
CommandCode parseCommand(char* command_string);
ErrorCode addLayer(LayerCollection* layer_collection, Layer* new_layer);
ErrorCode removeLayer(LayerCollection* layer_collection);
LayerCollection* createLayerCollection(size_t width, size_t height);
void freeLayer(Layer* layer);
void freeLayerCollection(LayerCollection* layer_collection);
void mergeAllLayers(LayerCollection* layer_collection);
int pixelRGBToGrayscaleValue(Pixel* pixel);
char convertGrayscaleToSymbol(int grayscale_value);

//-----------------------------------------------------------------------------
///
/// This function prints horizontal lines based on a given width.
///
/// @param width The width of the horizontal lines.
//
void printHorizontalLines(size_t width)
{
  for (size_t i = 0; i < width + EDGES; i++)
    printf("-");

  printf("\n");
}

//-----------------------------------------------------------------------------
///
/// This function prints the pixels of a given Layer.
///
/// @param layer The Layer.
//
void printLayer(Layer* layer)
{
  printHorizontalLines(layer->width_);

  for (size_t i = 0; i < layer->height_; i++)
  {
    printf("|");

    for (size_t j = 0; j < layer->width_; j++)
    {
      if (strcmp(layer->color_end_, "") != 0 && layer->pixels_[i][j] != ' ')
      {
        printf("\033%s%c%s", layer->color_end_, layer->pixels_[i][j], RESET_SEQUENCE);
      }
      else
      {
        printf("%c", layer->pixels_[i][j]);
      }
    }

    printf("|\n");
  }

  printHorizontalLines(layer->width_);
}

//-----------------------------------------------------------------------------
///
/// This function creates a layer and allocates it on the heap.
///
/// @param width The width of the new layer.
/// @param height The height of the new layer.
/// @param x The x coordinate of the new layer.
/// @param y The y cooridnate of the new layer.
///
/// @returns The new layer.
//
Layer* createLayer(size_t width, size_t height, size_t x, size_t y)
{
  Layer* new_layer = (Layer*) malloc(sizeof(Layer));

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
  new_layer->x_start_ = x;
  new_layer->y_start_ = y;
  new_layer->rotation_ = 0;
  new_layer->color_end_ = "";

  return new_layer;
}

//-----------------------------------------------------------------------------
///
/// This function creates a char matrix and allocates it on the heap.
///
/// @param width The width of the matrix.
/// @param height The height of the matrix.
///
/// @returns The new matrix.
//
char** createLayerPixels(size_t width, size_t height)
{
  char** pixels = (char**) calloc(height, sizeof(char*));

  if (pixels == NULL)
    return NULL;

  for (size_t i = 0; i < height; i++)
  {
    pixels[i] = (char*) malloc(width * sizeof(char) + NULL_BYTE);

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

//-----------------------------------------------------------------------------
///
/// This function retrievs input from the user as a char* and allocates it on the heap.
///
/// @returns The user input.
//
char* getInput()
{
  // Florin Zamfir, ESP WS 2022/23, A3
  // begin
  int i = 0;
  int character;
  int number_of_allocations = 1;
  char* user_input = (char*) malloc(DEFAULT_STRING_SIZE * sizeof(char));
  if (user_input == NULL)
    return NULL;

  for (i = 0; (character = getchar()) != '\n' && character != EOF ; i++)
  {
    user_input[i] = character;
    if ((i + 1) == (DEFAULT_STRING_SIZE * number_of_allocations))
    {
      char* temp_user_input = (char*) realloc(user_input, i + DEFAULT_STRING_SIZE + NULL_BYTE);

      if (temp_user_input == NULL)
      {
        free(user_input);
        return NULL;
      }

      user_input = temp_user_input;
      number_of_allocations++;
    }
  }

  user_input = realloc(user_input, (i + 1) + 1);
  if (user_input == NULL)
    return NULL;
  user_input[i] = '\0';
  return user_input;
  // end
}

//-----------------------------------------------------------------------------
///
/// This function returns the user input as an array of char* (words).
///
/// @returns The user input.
//
char** getUserInput()
{
  char* user_input = getInput();

  if (user_input == NULL)
  {
    return NULL;
  }

  char** tokens = (char**) malloc(10 * sizeof(char*)); // Works for max 10 words
  if (tokens == NULL)
  {
    free(user_input);
    return NULL;
  }

  unsigned token_length = 0;
  size_t i = 0;

  // https://stackoverflow.com/questions/3889992/how-does-strtok-split-the-string-into-tokens-in-c
  // begin
  for (char* token = strtok(user_input, " "); token != NULL; token = strtok(NULL, " "))
  {
  // end
    if (token == NULL)
    {
      free(user_input);
      return NULL;
    }

    token_length = strlen(token);

    tokens[i] = (char*) malloc(sizeof(char) * (token_length + NULL_BYTE));

    if (tokens[i] == NULL)
    {
      free(user_input);
      return NULL;
    }

    strcpy(tokens[i], token);

    i++;
  }

  tokens[i] = NULL; // Mark the end of the pointer array

  free(user_input);

  return tokens;
}

//-----------------------------------------------------------------------------
///
/// This function determines the length of a char* array.
///
/// @param data The char* array.
///
/// @returns The length of the array.
//
size_t getLength(char** data)
{
  size_t count = 0;

  for (size_t i = 0; data[i] != NULL; i++)
    count++;

  return count;
}

//-----------------------------------------------------------------------------
///
/// This function determines if a given is a number.
///
/// @param character The character.
///
/// @returns True if the character is a number, else false.
//
bool isCharNumber(char character)
{
  if (character < '0' || character > '9')
  {
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
///
/// This function determines if a given char* a number.
///
/// @param string The char*.
///
/// @returns True if the char* is a number, else false.
//
bool isStringumber(char* string)
{
  for (size_t char_index = 0; string[char_index] != '\0'; char_index++)
  {
    bool is_number = isCharNumber(string[char_index]);

    if (!is_number)
      return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
///
/// This function validates the user input.
///
/// @param user_input The user input.
///
/// @returns True if the user input is valid, else false.
//
bool isInitialUserInputValid(char** user_input)
{
  if (getLength(user_input) != 2)
  {
    return false;
  }

  for (size_t word_index = 0; user_input[word_index] != NULL; word_index++)
  {
    bool is_number = isStringumber(user_input[word_index]);

    if (!is_number)
    {
      return false;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
///
/// This function frees the user input.
///
/// @param user_input The user input.
///
//
void freeUserInput(char** user_input)
{
  if (user_input == NULL)
    return;

  for (size_t word = 0; user_input[word] != NULL; word++)
  {
    free(user_input[word]);
  }

  free(user_input);

  user_input = NULL;
}

//-----------------------------------------------------------------------------
///
/// This function frees a char* array.
///
/// @param pointer_array The char* array.
///
//
void freePointerArray(char** pointer_array)
{
  for (size_t word = 0; pointer_array[word] != NULL; word++)
  {
    free(pointer_array[word]);
  }

  free(pointer_array);
}

//-----------------------------------------------------------------------------
///
/// This function splits a string and retrieves the x and y coordinates.
///
/// @param x The x coordinate.
/// @param y The y coordinate.
/// @param coordinates The string containing the coordinates.
///
/// @returns The error code:
///   OUT_OF_MEMROY
///   CANNOT_PARSE_COORDINATES
///   SUCCESS
//
ErrorCode parseCoordinates(size_t* x, size_t* y, char* coordinates)
{
  char** coordinates_array = (char**) malloc(10 * sizeof(char*));// Max 10 coordinates strings

  if (coordinates_array == NULL)
    return OUT_OF_MEMORY;

  unsigned token_length = 0;
  size_t i = 0;

  // https://stackoverflow.com/questions/3889992/how-does-strtok-split-the-string-into-tokens-in-c
  // begin
  for (char* token = strtok(coordinates, ","); token != NULL; token = strtok(NULL, ","))
  {
  // end
    token_length = strlen(token);

    coordinates_array[i] = (char*) malloc(sizeof(char) * (token_length + NULL_BYTE));

    if (coordinates_array[i] == NULL)
    {
      freePointerArray(coordinates_array);
      return OUT_OF_MEMORY;
    }

    strcpy(coordinates_array[i], token);

    i++;
  }

  coordinates_array[i] = NULL; // Mark the end of the pointer array

  if (getLength(coordinates_array) != 2)
  {
    freePointerArray(coordinates_array);
    return CANNOT_PARSE_COORDINATES;
  }

  for (size_t coordinate_index = 0; coordinates_array[coordinate_index] != NULL; coordinate_index++)
  {
    bool is_number = isStringumber(coordinates_array[coordinate_index]);

    if (!is_number)
    {
      freePointerArray(coordinates_array);
      return CANNOT_PARSE_COORDINATES;
    }
  }

  *x = atoi(coordinates_array[0]);
  *y = atoi(coordinates_array[1]);

  freePointerArray(coordinates_array);

  return SUCCESS;
}

//-----------------------------------------------------------------------------
///
/// This function reads a .txt file and creates a layer of all the characters in it.
/// The new layer is added to the layer collection.
///
/// @param file The .txt file.
/// @param layer_collection The layer collection.
///
/// @returns The error code:
///   OUT_OF_MEMROY
///   SUCCESS
//
ErrorCode parseTextFile(FILE* file, LayerCollection* layer_collection)
{
  rewind(file);

  size_t width = 1;
  size_t height = 1;

  while (!feof(file))
  {
    if(fgetc(file) == '\n')
    {
      height++;
      continue;
    }

    if (height ==1)
      width++; // While reading the first line, determine its length.
  }

  Layer* new_layer = createLayer(width, height, 0 , 0);

  if (new_layer == NULL)
  {
    fclose(file);
    return OUT_OF_MEMORY;
  }

  rewind(file);

  int row = 0;
  int column = 0;
  char character = ' ';

  while (!feof(file))
  {
    character = getc(file);

    if (feof(file))
      break;

    if (character == '\n')
    {
      row++;
      column = 0;
      continue;
    }

    new_layer->pixels_[row][column] = character;
    column++;
  }

  addLayer(layer_collection, new_layer);

  fclose(file);
  return SUCCESS;
}

//-----------------------------------------------------------------------------
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
//
ErrorCode parseBinaryFile(FILE* file, LayerCollection* layer_collection)
{
  rewind(file);

  uint8_t header[54];
  fread(header, sizeof(uint8_t), 54, file);

  int width_pixels = *(int*)&header[18];  // 0x12 => 18
  int height_pixels = *(int*)&header[22]; // 0x16 => 22
  int padding = width_pixels % 4;

  int position_image_array = *(int*) &header[10]; // 0x0a => 10

  fseek(file, position_image_array, SEEK_SET);

  Layer* new_layer = createLayer(width_pixels, height_pixels, 0, 0);

  if (new_layer == NULL)
  {
    fclose(file);
    return OUT_OF_MEMORY;
  }

  for (int row = 0; row < height_pixels; row++)
  {
    for (int column = 0; column < width_pixels; column++)
  {
    unsigned char buffer[3];

    fread(buffer, sizeof(char), sizeof(buffer), file);

    Pixel pixel =
    {
      .blue_ = buffer[0],
      .green_ = buffer[1],
      .red_ = buffer[2],
    };

    new_layer->pixels_[(height_pixels - INDEX_OFFSET) - row][column] = convertGrayscaleToSymbol(pixelRGBToGrayscaleValue(&pixel));
   }

   fseek(file, padding, SEEK_CUR);
  }

  addLayer(layer_collection, new_layer);

  fclose(file);

  return SUCCESS;
}

//-----------------------------------------------------------------------------
///
/// This function loads a .bmp or .txt file and creates a layer containing their pixels.
///
/// @param parameters The parameters of the command.
/// @param layer_collection The layer collection.
///
/// @returns The error code:
///   OUT_OF_MEMROY
///   SUCCESS
//
ErrorCode load(char** parameters, LayerCollection* layer_collection)
{
  char* filepath = parameters[1];
  bool is_binary = false;
  size_t path_length = strlen(filepath);

  FILE* file = NULL;

  if (filepath[path_length - INDEX_OFFSET] == 't')
  {
    file = fopen(filepath, "r");

    return parseTextFile(file, layer_collection);
  }
  else
  {
    file = fopen(filepath, "rb");
    is_binary = true;

    if (file == NULL)
    return FILE_INVALID;

    char magic_number[2];

    fread(magic_number, sizeof(char), sizeof(magic_number), file);

    if (magic_number[0] != 'B' || magic_number[1] != 'M')
    {
      fclose(file);
      return FILE_INVALID;
    }

    return parseBinaryFile(file, layer_collection);
  }
}

//-----------------------------------------------------------------------------
///
/// This function resizes the drawing pane.
///
/// @param parameters The parameters of the command.
/// @param layer_collection The layer collection.
///
/// @returns The error code:
///   OUT_OF_MEMROY
///   SUCCESS
//
ErrorCode resize(char** parameters, LayerCollection* layer_collection)
{
  Layer* drawing_pane = layer_collection->layers_[0];

  size_t new_width = atoi(parameters[1]);
  size_t new_height = atoi(parameters[2]);

  // Tutorium A3 Musterloesung
  // begin
  for(size_t h = new_height; h < drawing_pane->height_; h++)
  {
    free(drawing_pane->pixels_[h]);
    drawing_pane->pixels_[h] = NULL;
  }

  char** temp = realloc(drawing_pane->pixels_, new_height * sizeof(char*));

  if(temp == NULL)
    return OUT_OF_MEMORY;

  drawing_pane->pixels_ = temp;

  for(size_t h = drawing_pane->height_; h < new_height; h++)
    drawing_pane->pixels_[h] = NULL;

  for (size_t h = 0; h < new_height; h++)
  {
    char* temp = realloc(drawing_pane->pixels_[h], new_width * sizeof(char));

    if(temp == NULL)
      return OUT_OF_MEMORY;

    drawing_pane->pixels_[h] = temp;
  }
  //end

  drawing_pane->width_ = new_width;
  drawing_pane->height_ = new_height;

  return SUCCESS;
}

//-----------------------------------------------------------------------------
///
/// This function draws a line on a new layer and adds it to the layer collection.
///
/// @param parameters The parameters of the command.
/// @param layer_collection The layer collection.
///
/// @returns The error code:
///   OUT_OF_MEMROY
///   SUCCESS
//
ErrorCode draw(char** parameters, LayerCollection* layer_collection)
{
  char to_draw = *parameters[1];
  size_t x = 0;
  size_t y = 0;

  ErrorCode error_code = parseCoordinates(&x, &y, parameters[2]);

  if (error_code == OUT_OF_MEMORY)
    return OUT_OF_MEMORY;

  x -= INDEX_OFFSET;
  y -= INDEX_OFFSET;

  unsigned angle = atoi(parameters[3]);
  unsigned number_to_draw = atoi(parameters[4]);

  Layer* new_layer = createLayer(number_to_draw, number_to_draw, x, y);

  if (new_layer == NULL)
    return OUT_OF_MEMORY;

  if (angle == 0)
  {
    for (size_t x_mod = 0; x_mod < number_to_draw; x_mod++)
    {
      new_layer->pixels_[0][x_mod] = to_draw;
    }
  }
  else if (angle == 45)
  {
    for (size_t mod = 0; mod < number_to_draw; mod++)
    {
      new_layer->pixels_[(number_to_draw - INDEX_OFFSET) - mod][mod] = to_draw;
    }
    new_layer->y_start_ -= (new_layer->height_ - INDEX_OFFSET);
  }
  else if (angle == 90)
  {
    for (size_t y_mod = 0; y_mod < number_to_draw; y_mod++)
    {
      new_layer->pixels_[y_mod][0] = to_draw;
    }

    new_layer->y_start_ -= (new_layer->height_ - INDEX_OFFSET);
  }
  else if (angle == 135)
  {
    for (long mod = 0; mod < number_to_draw; mod++)
    {
      new_layer->pixels_[mod][mod] = to_draw;
    }
    new_layer->x_start_ -= (new_layer->width_ - INDEX_OFFSET);
    new_layer->y_start_ -= (new_layer->height_ - INDEX_OFFSET);
  }
  else if (angle == 180)
  {
    for (size_t x_mod = 0; x_mod < number_to_draw; x_mod++)
    {
      new_layer->pixels_[0][x_mod] = to_draw;
    }
    new_layer->x_start_ -= (new_layer->width_ - INDEX_OFFSET);
  }
  else if (angle == 225)
  {
    for (size_t mod = 0; mod < number_to_draw; mod++)
    {
      new_layer->pixels_[mod][(number_to_draw - INDEX_OFFSET) - mod] = to_draw;
    }
    new_layer->x_start_ -= (new_layer->width_ - INDEX_OFFSET);
  }
  else if (angle == 270)
  {
   for (size_t y_mod = 0; y_mod < number_to_draw; y_mod++)
   {
     new_layer->pixels_[y_mod][0] = to_draw;
   }
  }
  else if (angle == 315)
  {
    for (size_t mod = 0; mod < number_to_draw; mod++)
    {
      new_layer->pixels_[mod][mod] = to_draw;
    }
  }

  addLayer(layer_collection, new_layer);

  return SUCCESS;
}

//-----------------------------------------------------------------------------
///
/// This function fills an area on a new layer and adds it to the layer collection.
///
/// @param parameters The parameters of the command.
/// @param layer_collection The layer collection.
/// @param undo_available Marks whether undo can be used after this action.
///
/// @returns The error code:
///   OUT_OF_MEMROY
///   SUCCESS
//
ErrorCode fill(char** parameters, LayerCollection* layer_collection, bool* undo_available)
{
  char to_draw = *parameters[3];

  size_t x_start = 0;
  size_t y_start = 0;

  size_t x_end = 0;
  size_t y_end = 0;

  if(parseCoordinates(&x_start, &y_start, parameters[1]))
    return OUT_OF_MEMORY;

  if(parseCoordinates(&x_end, &y_end, parameters[2]))
    return OUT_OF_MEMORY;

  x_start -= INDEX_OFFSET;
  y_start -= INDEX_OFFSET;

  size_t size = ((x_end - x_start) > (y_end - y_start)) ? (x_end - x_start) : (y_end - y_start);

  Layer* new_layer = createLayer(size, size, x_start, y_start);

  if (new_layer == NULL)
    return OUT_OF_MEMORY;

  for (size_t row = 0; row < y_end - y_start; row++)
  {
    for (size_t column = 0; column < x_end - x_start; column++)
    {
      new_layer->pixels_[row][column] = to_draw;
    }
  }

  addLayer(layer_collection, new_layer);

  *undo_available = true;

  return SUCCESS;
}

//-----------------------------------------------------------------------------
///
/// This function transposes a matrix.
///
/// @param layer The layer, whose pixels matrix will be transposed.
///
//
void transposeMatrix(Layer* layer)
{
  char transpose[layer->height_][layer->width_];

  // adapted from https://www.tutorialspoint.com/how-to-calculate-transpose-of-a-matrix-using-c-program
  // begin
  for (size_t row = 0;row < layer->height_;row++)
   for (size_t column = 0; column < layer->width_; column++)
      transpose[column][row] = layer->pixels_[row][column];
  // end

  for (size_t row = 0;row < layer->height_;row++)
   for (size_t column = 0; column < layer->width_; column++)
      layer->pixels_[row][column] = transpose[row][column];
}

//-----------------------------------------------------------------------------
///
/// This function switches the rows of a matrix.
///
/// @param layer The layer, whose matrix' rows will be transposed.
///
//
void switchRows(Layer* layer)
{
  size_t start = 0;
  size_t end = layer->height_ - INDEX_OFFSET;

  while (start < end)
  {
    char* tmp = layer->pixels_[start];
    layer->pixels_[start] = layer->pixels_[end];
    layer->pixels_[end] = tmp;
    start++;
    end--;
  }
}

//-----------------------------------------------------------------------------
///
/// This function performs the rotation logic of a given layer.
///
/// @param layer The layer to be rotated.
///
//
void rotateLayer(Layer* layer)
{
  transposeMatrix(layer);

  if (layer->rotation_ == 0)
  {
    layer->rotation_ = 1;
    layer->y_start_ -= (layer->height_ - INDEX_OFFSET);
  }
  else if (layer->rotation_ == 1)
  {
    layer->rotation_ = 2;
    layer->y_start_ += (layer->height_ - INDEX_OFFSET);
    layer->x_start_ -= (layer->width_ - INDEX_OFFSET);
  }
  else if (layer->rotation_ == 2)
  {
    layer->rotation_ = 3;
    layer->x_start_ += (layer->width_ - INDEX_OFFSET);
  }
  else if (layer->rotation_ == 3)
  {
    // Reset
    layer->rotation_ = 0;
  }
}

//-----------------------------------------------------------------------------
///
/// This function rotates all layers 90 degrees counter clockwise.
///
/// @param layer_collection The layer collection.
///
//
void rotate(LayerCollection* layer_collection)
{
  for (size_t layer_i = 1; layer_i < layer_collection->count_; layer_i++)
  {
    rotateLayer(layer_collection->layers_[layer_i]);
  }
}

//-----------------------------------------------------------------------------
///
/// This function removes the last layer in the layer collection.
///
/// @param layer_collection The layer collection.
///
//
void undo(LayerCollection* layer_collection)
{
  removeLayer(layer_collection);
}

//-----------------------------------------------------------------------------
///
/// This function saves a layer to a .txt file.
///
/// @param filename The name of the save file.
/// @param layer The layer to be saved.
///
//
void saveLayerToFile(char* filename, Layer* layer)
{
  FILE* save_file = fopen(filename,"w");

  for (size_t row = 0; row < layer->height_; row++)
  {
    fprintf(save_file, "%s", layer->pixels_[row]);
    fprintf(save_file, "\n");
  }

  fclose(save_file);
}

//-----------------------------------------------------------------------------
///
/// This function saves all the layers to a .txt file.
///
/// @param parameters The parameters of the command.
/// @param layer_collection The layer collection.
///
/// @returns The error code:
///   OUT_OF_MEMROY
///   SUCCESS
//
ErrorCode save(char** parameters, LayerCollection* layer_collection)
{
  char* filename = parameters[1];

  mergeAllLayers(layer_collection);

  saveLayerToFile(filename, layer_collection->layers_[0]);

  return SUCCESS;
}

//-----------------------------------------------------------------------------
///
/// This function parses a string and determines which command to be executed.
///
/// @param command_string The string to be parsed.
///
/// @returns The command code:
///   OUT_OF_MEMROY
///   SUCCESS
//
CommandCode parseCommand(char* command_string)
{
  if (strcmp(command_string, "load") == 0)
  {
    return LOAD;
  }
  else if (strcmp(command_string, "resize") == 0)
  {
    return RESIZE;
  }
  else if (strcmp(command_string, "draw") == 0)
  {
    return DRAW;
  }
  else if (strcmp(command_string, "fill") == 0)
  {
    return FILL;
  }
  else if (strcmp(command_string, "rotate") == 0)
  {
    return ROTATE;
  }
  else if (strcmp(command_string, "undo") == 0)
  {
    return UNDO;
  }
  else if (strcmp(command_string, "save") == 0)
  {
    return SAVE;
  }
  else if (strcmp(command_string, "quit") == 0)
  {
    return QUIT;
  }
  else if (strcmp(command_string, "color") == 0)
  {
    return COLOR;
  }
  else
  {
    return INVALID_COMMAND;
  }
}

//-----------------------------------------------------------------------------
///
/// This function sets the color of a certain layer.
///
/// @param parameters The command parameters.
/// @param layer The layer, whose color is set.
///
//
void color(char** parameters, Layer* layer)
{
  char* color = parameters[1];

  if (strcmp(color, "black") == 0)
    layer->color_end_ = "[30m";
  else if (strcmp(color, "red") == 0)
    layer->color_end_ = "[31m";
  else if (strcmp(color, "green") == 0)
    layer->color_end_ = "[32m";
  else if (strcmp(color, "yellow") == 0)
    layer->color_end_ = "[33m";
  else if (strcmp(color, "blue") == 0)
    layer->color_end_ = "[34m";
  else if (strcmp(color, "magenta") == 0)
    layer->color_end_ = "[35m";
  else if (strcmp(color, "cyan") == 0)
    layer->color_end_ = "[36m";
  else if (strcmp(color, "white") == 0)
    layer->color_end_ = "[37m";
  else
    layer->color_end_ = "[0m";
}

//-----------------------------------------------------------------------------
///
/// This function adds a new layer to the layer collection.
///
/// @param layer_collection The layer collection.
/// @param new_layer The new layer to be added.
///
/// @returns The error code:
///   OUT_OF_MEMROY
///   SUCCESS
//
ErrorCode addLayer(LayerCollection* layer_collection, Layer* new_layer)
{
  Layer** tmp = (Layer**) realloc(layer_collection->layers_, layer_collection->count_ * sizeof(Layer*) + sizeof(Layer*));

  if (tmp == NULL)
    return OUT_OF_MEMORY;

  layer_collection->layers_ = tmp;
  tmp = NULL;

  layer_collection->layers_[layer_collection->count_] = new_layer;
  layer_collection->count_++;

  return SUCCESS;
}

//-----------------------------------------------------------------------------
///
/// This function removes the last layer from the layer collection.
///
/// @param layer_collection The layer collection.
///
/// @returns The error code:
///   OUT_OF_MEMROY
///   SUCCESS
//
ErrorCode removeLayer(LayerCollection* layer_collection)
{
  freeLayer(layer_collection->layers_[layer_collection->count_ - INDEX_OFFSET]);

  layer_collection->count_--;
  Layer** tmp = (Layer**) realloc(layer_collection->layers_, layer_collection->count_ * sizeof(Layer*));

  if (tmp == NULL)
    return OUT_OF_MEMORY;

  layer_collection->layers_ = tmp;

  return SUCCESS;
}

//-----------------------------------------------------------------------------
///
/// This function creates the layer collection and allocates it on the heap.
///
/// @param width The width of the drawing pane.
/// @param height The height of the drawing pane.
///
/// @returns The layer collection.
//
LayerCollection* createLayerCollection(size_t width, size_t height)
{
  LayerCollection* layer_collection = (LayerCollection*) malloc(sizeof(LayerCollection));

  if (layer_collection == NULL)
    return NULL;

  layer_collection->layers_ = (Layer**) malloc(sizeof(Layer*));

  if (layer_collection == NULL)
  {
    free(layer_collection);
    return NULL;
  }

  Layer* drawing_pane = createLayer(width, height, 0, 0);

  if (drawing_pane == NULL)
  {
    free(layer_collection);
    return NULL;
  }

  layer_collection->layers_[0] = drawing_pane;
  layer_collection->count_ = 1;

  return layer_collection;
}

//-----------------------------------------------------------------------------
///
/// This function frees a given layer.
///
/// @param layer The layer to be freed.
///
//
void freeLayer(Layer* layer)
{
  for (size_t row = 0; row < layer->height_; row++)
  {
    free(layer->pixels_[row]);
  }

  free(layer->pixels_);
  free(layer);
}

//-----------------------------------------------------------------------------
///
/// This function frees the layer collection.
///
/// @param layer_collection The layer collection.
///
//
void freeLayerCollection(LayerCollection* layer_collection)
{
  for (size_t layer_i = 0; layer_i < layer_collection->count_; layer_i++)
  {
    freeLayer(layer_collection->layers_[layer_i]);
  }

  free(layer_collection->layers_);

  free(layer_collection);
}

//-----------------------------------------------------------------------------
///
/// This function merges all the layers in the layer collection into one layer.
///
/// @param layer_collection The layer collection.
///
//
void mergeAllLayers(LayerCollection* layer_collection)
{
  Layer* drawing_pane = layer_collection->layers_[0];

  // Tutorium A3 Musterloesung
  // begin
  for(size_t h = 0; h < drawing_pane->height_; h++)
  {
    for(size_t w = 0; w < drawing_pane->width_; w++)
      drawing_pane->pixels_[h][w] = ' ';
  }

  for (size_t layer_i = 1; layer_i < layer_collection->count_; layer_i++)
  {
    Layer* current_layer = layer_collection->layers_[layer_i];
    long field_current_y = (long) current_layer->y_start_;

    for (size_t h = 0; h < current_layer->height_; h++)
    {
      long field_current_x = (long) current_layer->x_start_;
      for (size_t w = 0; w < current_layer->width_; w++)
      {
        if (field_current_y < 0 || (long) drawing_pane->height_ -INDEX_OFFSET < field_current_y)
          break;

        if (field_current_x < 0 || (long) drawing_pane->width_ -INDEX_OFFSET < field_current_x)
          break;

        if(current_layer->pixels_[h][w] != ' ')
          drawing_pane->pixels_[field_current_y][field_current_x] = current_layer->pixels_[h][w];
        field_current_x++;

        if ((long) drawing_pane->width_ < field_current_x || field_current_x < 0)
          break;
      }
      field_current_y++;
      if ((long) drawing_pane->height_ < field_current_y)
       break;
    }
  }
  // end
}

//-----------------------------------------------------------------------------
///
/// This function converts the rgb values of a pixel to grayscale.
///
/// @param pixel The pixel.
///
/// @returns The grayscale value.
//
int pixelRGBToGrayscaleValue(Pixel* pixel)
{
  double blue = pixel->blue_;
  double green = pixel->green_;
  double red = pixel->red_;
  double grayscale_value = red * 0.2126 + green * 0.7152 + blue * 0.0722;

  return grayscale_value;
}

//-----------------------------------------------------------------------------
///
/// This function converts the grayscale value to the coresponding ASCII symbol.
///
/// @param grayscale_value The grayscale value.
///
/// @returns The ASCII symbol.
///
//
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

//-----------------------------------------------------------------------------
///
/// This is the program entry point.
///
/// @returns The error code:
///   OUT_OF_MEMORY
///   SUCCESS
///
//
int main(void)
{
  char** user_input = NULL;
  bool invalid_input = false;
  bool undo_available = false;

  printf("> Please Enter the size of the Drawing Pane (width, height): ");
  while (true)
  {
    if (invalid_input)
      printf("Invalid Input\n> ");

    user_input = getUserInput();

    if (user_input == NULL)
    {
      printf("Out of Memory!");
      return OUT_OF_MEMORY;
    }

    if (!isInitialUserInputValid(user_input))
    {
      invalid_input = true;
      freeUserInput(user_input);
      continue;
    }
    break;
  }

  size_t width = atoi(user_input[0]);
  size_t height = atoi(user_input[1]);
  LayerCollection* layer_collection = createLayerCollection(width, height);
  if (layer_collection == NULL)
  {
    printf("Out of Memory!");
    return OUT_OF_MEMORY;
  }

  Layer* drawing_pane = layer_collection->layers_[0];
  printLayer(drawing_pane);

  while (true)
  {
    freeUserInput(user_input);
    printf("> ");
    user_input = getUserInput();

    if (user_input == NULL)
    {
      printf("Out of Memory!");
      freeLayerCollection(layer_collection);
      return OUT_OF_MEMORY;
    }

    if (strcmp(user_input[0], "quit") == 0)
      break;

    CommandCode command = parseCommand(user_input[0]);
    ErrorCode error_code = SUCCESS;
    switch (command)
    {
    case LOAD:
      error_code = load(user_input, layer_collection);
      undo_available = true;
      break;
    case RESIZE:
      error_code = resize(user_input, layer_collection);
      undo_available = false;
      break;
    case DRAW:
      error_code = draw(user_input, layer_collection);
      undo_available = true;
      break;
    case FILL:
      error_code = fill(user_input, layer_collection, &undo_available);
      break;
    case ROTATE:
      if (layer_collection->count_ > 1)
        rotate(layer_collection);
      else
      {
        printf("Command can not be executed in this state!\n");
        continue;
      }
      break;
    case UNDO:
      if (undo_available)
      {
        undo(layer_collection);
        undo_available = false;
      }
      else
      {
        printf("Command can not be executed in this state!\n");
        continue;
      }
      break;
    case SAVE:
      error_code = save(user_input, layer_collection);
      undo_available = false;
      continue;
    case COLOR:
      color(user_input, drawing_pane);
      undo_available = false;
      break;
    default:
      printf("Invalid Input\n");
      break;
    }

    if (error_code == OUT_OF_MEMORY)
    {
      freeLayerCollection(layer_collection);
      freeUserInput(user_input);
      printf("Out of Memory!");
      return OUT_OF_MEMORY;
    }
    else if (error_code == FILE_INVALID)
    {
      printf("Invalid File\n");
      continue;
    }
    mergeAllLayers(layer_collection);
    printLayer(drawing_pane);
  }
  freeLayerCollection(layer_collection);
  freeUserInput(user_input);
  return SUCCESS;
}
