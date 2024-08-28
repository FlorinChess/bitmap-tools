#include "definitions.h"
#include "bitmap.h"
#include "layer.h"
#include "utils.h"

#ifdef DEBUG
#include "debug.h"
#endif

size_t max_message_length_ = 0;
LayerCollection layer_collection_;

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

ErrorCode getMessageMaxLength(char* file_path)
{
  FILE* file = fopen(file_path, "rb");

  if (file == NULL)
    return CANNOT_OPEN_FILE;

  BitmapFileHeader file_header;
  BitmapInfoHeader info_header;
  
  if (fread(&file_header, sizeof(char), sizeof(BitmapFileHeader), file) != sizeof(BitmapFileHeader))
  {
    fclose(file);
    return INVALID_FILE;
  } 
    
  if (fread(&info_header, sizeof(char), sizeof(BitmapInfoHeader), file) != sizeof(BitmapInfoHeader))
  {
    fclose(file);
    return INVALID_FILE;
  }

  if (strncmp(file_header.magic_word_, BMP_MAGIC_WORD, 2) != 0)
  {
    fclose(file);
    return INVALID_FILE;
  }

  max_message_length_ = (((info_header.bitmap_width_ * info_header.bitmap_height_) * 3) / 8) - BMP_PREFIX_SIZE;

  fclose(file);

  return SUCCESS;
}

void printHelpMessage()
{
  printf(
    "Usage: bmp <option> <bmp-file(s)>\n"
    "Encode and decode hidden messages into bitmap image files. Can be used to convert bitmap files into ASCII art.\n"
    "Options:\n"
    " -h, --help       Display this help message\n"
    " -a, --ascii      Convert bitmap images to ASCII art\n"
    //" -g, --grayscale  Turn bitmap images into grayscale\n"
    " -d, --decode     Decode secret message from the bitmap file\n"
    " -e, --encode     Encode secret message into the bitmap file\n"
    );
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

ErrorCode convertFilesToASCII(char* file_paths[])
{
  for (size_t i = 0; file_paths[i] != NULL; ++i)
  {
    handleIfError(convertBitmapToASCII(file_paths[i]));

    char* filename = getFilePathWithoutExtension(file_paths[i]);

    handleIfError(saveLayerToFile(filename, i));
  }

  return SUCCESS;
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
    handleIfError(checkParameters(argv + 2, argc));

    handleIfError(decode(argv[2]));

  }
  else if (strcmp(option, "-e") == 0 || strcmp(option, "--encode") == 0)
  {
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
