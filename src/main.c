#include "definitions.h"
#include "bitmap.h"
#include "layer.h"
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

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

void lengthToString(size_t length, char buffer[4])
{
  buffer[0] = '0' + (length / 1000) % 10;
  buffer[1] = '0' + (length / 100) % 10;
  buffer[2] = '0' + (length / 10) % 10;
  buffer[3] = '0' + (length % 10);
}

ErrorCode parseBinaryFile(char* file_path)
{
  FILE* file = fopen(file_path, "rb");

  if (file == NULL)
    return CANNOT_OPEN_FILE;

  BitmapFileHeader file_header;
  BitmapInfoHeader info_header;
  
  ErrorCode error_code = readBitmapHeaders(file, &file_header, &info_header);

  if (error_code != SUCCESS)
  {
    fclose(file);
    return error_code;
  }

  Layer* new_layer = createLayer(info_header.bitmap_width_, info_header.bitmap_height_);
  if (new_layer == NULL)
  {
    fclose(file);
    return OUT_OF_MEMORY;
  }

  int padding = info_header.bitmap_width_ % 4;
  fseek(file, file_header.pixel_array_offset_, SEEK_SET);
  for (int row = 0; row < info_header.bitmap_height_; row++)
  {
    for (int column = 0; column < info_header.bitmap_width_; column++)
    {
      Pixel pixel;
      if (fread(&pixel, sizeof(char), sizeof(Pixel), file) != sizeof(Pixel))
      {
        free(new_layer);
        fclose(file);
        return INVALID_FILE;
      }

      printf("b:%u, g:%u, r:%u\n", pixel.blue_, pixel.green_, pixel.red_);

      new_layer->pixels_[(info_header.bitmap_height_ - INDEX_OFFSET) - row][column] = convertGrayscaleToSymbol(convertPixelToGrayscale(&pixel));
   }

   fseek(file, padding, SEEK_CUR);
  }
  addLayer(new_layer);

  fclose(file);

  return SUCCESS;
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

bool isCharNumber(char character)
{
  if (character < '0' || character > '9')
  {
    return false;
  }

  return true;
}

int cp(const char *to, const char *from)
{
  int fd_to, fd_from;
  char buf[4096];
  ssize_t nread;
  int saved_errno;

  fd_from = open(from, O_RDONLY);
  if (fd_from < 0)
    return -1;

  fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
  if (fd_to < 0)
    goto out_error;

  while (nread = read(fd_from, buf, sizeof buf), nread > 0)
  {
    char *out_ptr = buf;
    ssize_t nwritten;

    do 
    {
      nwritten = write(fd_to, out_ptr, nread);

      if (nwritten >= 0)
      {
        nread -= nwritten;
        out_ptr += nwritten;
      }
      else if (errno != EINTR)
      {
        goto out_error;
      }
    } 
    while (nread > 0);
  }

  if (nread == 0)
  {
    if (close(fd_to) < 0)
    {
      fd_to = -1;
      goto out_error;
    }
    close(fd_from);

    /* Success! */
    return 0;
  }

out_error:
  saved_errno = errno;

  close(fd_from);
  if (fd_to >= 0)
    close(fd_to);

  errno = saved_errno;
  return -1;
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

char* getFileExtension(const char* file_path) 
{
  char *dot = strchr(file_path, '.');
  if(!dot || dot == file_path) 
  {
    return "";
  }
  return dot + 1;
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
