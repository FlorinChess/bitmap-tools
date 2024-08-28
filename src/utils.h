#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define INDEX_OFFSET 1

///
/// @brief This function converts a numeric value into a 4-digit string representation.
///
/// @param length The number value representing the length of the message.  
/// @param buffer The buffer in which the resulting string is stored in.
///
void lengthToString(size_t length, char buffer[4]);

///
/// @brief This function checks if a character represents a digit from 0 to 9.
///
/// @param character The character to check.
///
/// @return True if the parameter represents a digit from 0 to 9.
///
bool isCharNumber(char character);

///
/// @brief This function modifies the given buffer and returns the fully qualified file path of a given file without the file extension.
///
/// @param file_path The full file path containing the file extension.
///
/// @return The full file path without the extension.
///
char* getFilePathWithoutExtension(char* file_path);

///
/// @brief This function returns the file extension from a fully qualified file path.
///
/// @param file_path The file path to extract the extension from.
///
/// @return A char pointer that point to the part of the given file path that contains only the file extension. 
///
char* getFileExtension(const char* file_path);

///
/// @brief This function copies a file from a given file path.
///
/// @param from The file to copy.
/// @param to This file to save the copy into.
///
/// @return 0 on success, -1 on failure.
///
int copy(const char* from, const char* to);

#endif // UTILS_H
