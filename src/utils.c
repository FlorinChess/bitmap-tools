#include "utils.h"
#include <string.h>

void lengthToString(size_t length, char buffer[4])
{
  buffer[0] = '0' + (length / 1000) % 10;
  buffer[1] = '0' + (length / 100) % 10;
  buffer[2] = '0' + (length / 10) % 10;
  buffer[3] = '0' + (length % 10);
}

bool isCharNumber(char character)
{
  if (character < '0' || character > '9')
  {
    return false;
  }

  return true;
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

char* getFileExtension(const char* file_path) 
{
  char* dot = strchr(file_path, '.');
  if(!dot || dot == file_path) 
  {
    return "";
  }
  return dot + 1;
}

int copy(const char* from, const char* to)
{
  char buffer[4096];
  ssize_t nread;
  int saved_errno;

  int fd_from = open(from, O_RDONLY);
  if (fd_from < 0)
    return -1;

  int fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
  if (fd_to < 0)
    goto out_error;

  while (nread = read(fd_from, buffer, sizeof(buffer)), nread > 0)
  {
    char *out_ptr = buffer;
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
