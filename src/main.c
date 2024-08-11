#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "definitions.h"
#include "error.h"
#include "bitmap.h"

size_t max_message_length_ = 0;
LayerCollection layer_collection_;

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


///
/// This function retrievs input from the user as a char* and allocates it on the heap.
///
/// @returns The user input.
//
char* getInput()
{
  int i = 0;
  int character;
  int number_of_allocations = 1;
  char* user_input = malloc(DEFAULT_STRING_SIZE * sizeof(char));
  if (user_input == NULL)
    return NULL;

  for (i = 0; (character = getchar()) != '\n' && character != EOF ; i++)
  {
    user_input[i] = character;
    if ((i + 1) == (DEFAULT_STRING_SIZE * number_of_allocations))
    {
      char* temp_user_input = realloc(user_input, i + DEFAULT_STRING_SIZE + NULL_BYTE);

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
}


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

  char** tokens = malloc(10 * sizeof(char*)); // Works for max 10 words
  if (tokens == NULL)
  {
    free(user_input);
    return NULL;
  }

  unsigned token_length = 0;
  size_t i = 0;

  for (char* token = strtok(user_input, " "); token != NULL; token = strtok(NULL, " "))
  {
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

void freePointerArray(char** pointer_array)
{
  for (size_t word = 0; pointer_array[word] != NULL; word++)
  {
    free(pointer_array[word]);
  }

  free(pointer_array);
}

ErrorCode parseTextFile(FILE* file)
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

  Layer* new_layer = createLayer(width, height);

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

  addLayer(new_layer);

  fclose(file);
  return SUCCESS;
}

ErrorCode parseBinaryFile(char* file_path)
{
  FILE* file = fopen(file_path, "rb");

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

  int width_pixels =  *(int*)&header[0x12]; // 0x12 => 18
  int height_pixels = *(int*)&header[0x16]; // 0x16 => 22
  int padding = width_pixels % 4;

  int position_image_array = *(int*) &header[0x0a]; // 0x0a => 10

  fseek(file, position_image_array, SEEK_SET);

  Layer* new_layer = createLayer(width_pixels, height_pixels);

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
      fread_return = fread(buffer, sizeof(char), sizeof(buffer), file);

      if (fread_return != sizeof(buffer))
      {
        free(new_layer);
        fclose(file);
        return INVALID_FILE;
      }

      Pixel pixel;
      pixel.blue_ = buffer[0];
      pixel.green_ = buffer[1];
      pixel.red_ = buffer[2];

      new_layer->pixels_[(height_pixels - INDEX_OFFSET) - row][column] = convertGrayscaleToSymbol(pixelRGBToGrayscaleValue(&pixel));
   }

   fseek(file, padding, SEEK_CUR);
  }
  addLayer(new_layer);

  fclose(file);

  return SUCCESS;
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

ErrorCode addLayer(Layer* new_layer)
{
  if (layer_collection_.count_ == 10)
    return MAX_LAYER_COLLECTION_CAPACITY_REACHED;

  layer_collection_.layers_[layer_collection_.count_] = new_layer;
  layer_collection_.count_++;

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

int pixelRGBToGrayscaleValue(Pixel* pixel)
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

char* getFilePathWithoutExtension(char* file_path)
{
  for (size_t i = 0; file_path[i] != '\0'; i++)
  {
    if (file_path[i] == '.')
    {
      file_path[i] = '\0';
      break;
    }
  }
  return file_path;
}

void printBinary(unsigned char byte) 
{
  // Iterate over each bit from most significant to least significant
  for (int i = 7; i >= 0; i--) {
      // Print '1' if the i-th bit is set, '0' otherwise
      printf("%c", (byte & (1 << i)) ? '1' : '0');
  }
  printf("\n");
}

ErrorCode decode(char* file_path)
{
  FILE* file = fopen(file_path, "rb");

  if (file == NULL)
    return CANNOT_OPEN_FILE;

  uint8_t header[BMP_HEADER_SIZE];
  size_t fread_return = fread(header, sizeof(uint8_t), BMP_HEADER_SIZE, file);

  if (fread_return != sizeof(header))
  {
    fclose(file);
    return INVALID_FILE;
  }

  int position_image_array = *(int*)&header[0x0a]; // 0x0a => 10
  fseek(file, position_image_array, SEEK_SET);

  uint8_t buffer[BMP_PREFIX_SIZE * 8];
  fread_return = fread(buffer, sizeof(char), sizeof(buffer), file);

  char prefix[BMP_PREFIX_SIZE] = {0};
  int prefix_index = 0;
  int shift = 7;

  for (size_t i = 0; i < BMP_PREFIX_SIZE * 8; i++)
  { 
    // printf("prefix[%d] = ", prefix_index);
    // printBinary(prefix[prefix_index]);
    printf("buffer[%zu] = ", i);
    printBinary(buffer[i]);
    
    prefix[prefix_index] |= (buffer[i] & 0x01) << shift;

    shift = (shift == 0) ? 7 : shift - 1;

    if (shift == 7)
    {
      // printBinary(prefix[prefix_index]);
      prefix_index++;
    }
  }

  // for (int i = 0; i < BMP_PREFIX_SIZE; i++) 
  // {
  //   printf("%02x ", (unsigned char)prefix[i]);
  // }

  printf("\n");
  
  fclose(file);

  return SUCCESS;
}

ErrorCode encode(char* file_path, char* message)
{
  FILE* file = fopen(file_path, "rb+");

  if (file == NULL)
    return CANNOT_OPEN_FILE;

  BitmapFileHeader file_header;
  BitmapInfoHeader info_header;

  fread(&file_header, sizeof(BitmapFileHeader), 1, file);
  fread(&info_header, sizeof(info_header), 1, file);

  uint8_t header[BMP_HEADER_SIZE];
  size_t fread_return = fread(header, sizeof(uint8_t), BMP_HEADER_SIZE, file);

  if (fread_return != sizeof(header))
  {
    fclose(file);
    return INVALID_FILE;
  }

  int position_image_array = *(int*)&header[0x0a]; // 0x0a => 10
  printf("position = %p\n", (void*)position_image_array);
  fseek(file, position_image_array, SEEK_SET);

  int message_length = strlen(message);
  size_t size_to_encode_in_bits = (BMP_PREFIX_SIZE + message_length) * 8;

  printf("String to encode: \"BMP%04d%s\"\n", message_length, message);
  printf("Actual length to encode in bytes: %ld\n", size_to_encode_in_bits);

  char prefix[BMP_PREFIX_SIZE] = {0};

  memcpy(prefix, "BMP", 3);
  memcpy(prefix + 3, &message_length, sizeof(int));

  for (int i = 0; i < BMP_PREFIX_SIZE; i++) 
  {
    printf("%02x ", (unsigned char)prefix[i]);
  }

  uint8_t buffer[size_to_encode_in_bits];
  fread_return = fread(buffer, sizeof(char), sizeof(buffer), file);

  int prefix_index = 0;
  int shift = 7;
  int message_index = 0;

  for (size_t i = 0; i < size_to_encode_in_bits; i++)
  {
    if (prefix_index < BMP_PREFIX_SIZE)
    {
      if (shift == 7)
      {
        printf("Char to encode = %c ", prefix[prefix_index]);
        printBinary(prefix[prefix_index]);
      }

      uint8_t bit_to_encode = (prefix[prefix_index] >> shift) & 0x01;
      printf("Bit to encode = %d\n", bit_to_encode);
      printf("buffer[%zu] before encoding = ", i);
      printBinary(buffer[i]);

      // Clear bit if already set
      buffer[i] = buffer[i] & ~0x01;

      // Set the last bit as the given bit to encode
      buffer[i] = buffer[i] | bit_to_encode;

      printf("buffer[%zu] after encoding =  ", i);
      printBinary(buffer[i]);

      shift = (shift == 0) ? 7 : shift - 1;

      if (shift == 7)
      {
        prefix_index++;
      }
    }

    if (message_index < message_length)
    {
      if (shift == 7)
      {
        printf("Char to encode = %c ", message[message_index]);
        printBinary(message[message_index]);
      }

      uint8_t bit_to_encode = (message[message_index] >> shift) & 0x01;
      printf("Bit to encode = %d\n", bit_to_encode);
      printf("buffer[%zu] before encoding = ", i);
      printBinary(buffer[i]);

      // Clear bit if already set
      buffer[i] = buffer[i] & ~0x01;

      // Set the last bit as the given bit to encode
      buffer[i] = buffer[i] | bit_to_encode;

      printf("buffer[%zu] after encoding =  ", i);
      printBinary(buffer[i]);

      shift = (shift == 0) ? 7 : shift - 1;

      if (shift == 7)
        message_index++;
    }
  }

  // Move to where the array starts
  fseek(file, position_image_array, SEEK_SET);

  // Write the slightly modified data
  fwrite(buffer, sizeof(char), sizeof(buffer), file);  

  // Flush the changes
  fflush(file);

  fclose(file);

  return SUCCESS;
}

ErrorCode getMessageMaxLength(char* file_path)
{
  FILE* file = fopen(file_path, "rb");

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

  unsigned width_pixels =  *(int*)&header[0x12]; // 0x12 => 18
  unsigned height_pixels = *(int*)&header[0x16]; // 0x16 => 22

  max_message_length_ = (((width_pixels * height_pixels) * 3) / 8) - BMP_PREFIX_SIZE;

  fclose(file);

  return SUCCESS;
}

ErrorCode convertFilesToASCII(char* file_paths[])
{
  for (size_t i = 0; file_paths[i] != NULL; ++i)
  {
    handleIfError(parseBinaryFile(file_paths[i]));

    char* filename = getFilePathWithoutExtension(file_paths[i]);

    handleIfError(saveLayerToFile(filename, i));
  }

  return SUCCESS;
}

void printHelpMessage()
{
  printf(
    "Usage: converter <option> <bmp-file(s)>\n"
    "Converts a bitmap file into ASCII art and saves in a text file.\n"
    "Options:\n"
    " -h, --help       Display this help message\n"
    " -a, --ascii      Convert bitmap images to ASCII art\n"
    " -g, --grayscale  Turn bitmap images into grayscale\n"
    " -d, --decode     Decode secret message from the bitmap file\n"
    " -e, --encode     Encode secret message into the bitmap file\n"
    );
}

char* getFileExtension(const char* file_path) 
{
  char *dot = strchr(file_path, '.');
  if(!dot || dot == file_path) 
  {
    return "";
  }
  return dot + 1;
}

void handleIfError(ErrorCode error_code)
{
  switch (error_code)
  {
  case INVALID_USAGE:
    printf("Invalid usage!\n\n");
    break;
  case INVALID_FILE:
    printf("Invalid file(s)!\n\n");
    break;
  case OUT_OF_MEMORY:
    printf("Program ran out of memory!\n\n");
    break;
  case CANNOT_OPEN_FILE:
    printf("Cannot open file!\n\n");
    break;
  case MAX_LAYER_COLLECTION_CAPACITY_REACHED:
    printf("Cannot open more than 10 files!\n\n");
    break;
  case BITMAP_FILE_CORRUPTED:
    printf("Bitmap file corrupted!\n\n");
    break;
  case SUCCESS:
    return;
    break;
  default:
    printf("Unknown error occured!\n\n");
    break;
  }
  exit(error_code);
}

ErrorCode checkParameters(char* parameters[], const size_t count)
{
  if (count < 3)
    return INVALID_USAGE;

  for (size_t i = 0; parameters[i] != NULL; i++)
  {
    char* extension = getFileExtension(parameters[i]);

    if (strcmp(extension, "bmp") != 0)
    {
      printf("Wrong file format! (%s)\n", parameters[i]);
      return INVALID_FILE;
    }
  }

  return SUCCESS;
}

bool isUserInputValid(char* user_input)
{
  size_t length = strlen(user_input);

  if (length == 0)
  {
    printf("No message provided!\n\n");
    free(user_input);
    return false;
  }

  if (length > max_message_length_)
  {
    printf("Provided message is too long!\n\n");
    free(user_input);
    return false;
  }

  return true;
}

int main(int argc, char* argv[])
{
  if (argc == 1)
  {
    printf("Invalid usage!\n\n");
    printHelpMessage();
    return -1;
  }

  char* option = argv[1];

  if (strcmp(option, "-h") == 0 || strcmp(option, "--help") == 0 || strcmp(option, "help") == 0)
  {
    printHelpMessage();
    return 0;
  }
  else if (strcmp(option, "-a") == 0 || strcmp(option, "--ascii") == 0)
  {
    handleIfError(checkParameters(argv + 2, argc));
    
    handleIfError(convertFilesToASCII(argv + 2));
  }
  else if (strcmp(option, "-d") == 0 || strcmp(option, "--decode") == 0)
  {
    printf("decode\n");

    handleIfError(checkParameters(argv + 2, argc));

    handleIfError(decode(argv[2]));

  }
  else if (strcmp(option, "-e") == 0 || strcmp(option, "--encode") == 0)
  {
    printf("encode\n");

    handleIfError(checkParameters(argv + 2, argc));

    handleIfError(getMessageMaxLength(argv[2])); 

    char* user_input = NULL;
    do
    {
      printf("Input a message to encode into the bitmap image (%zu characters max) = ", max_message_length_);
      user_input = getInput();

    } while (!isUserInputValid(user_input));
    
    handleIfError(encode(argv[2], user_input));

    free(user_input);
  }
  else
  {
    printf("Invalid option!\n\n");
    printHelpMessage();
    return INVALID_OPTION;
  }
}
