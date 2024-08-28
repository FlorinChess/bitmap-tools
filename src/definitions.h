#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define DEFAULT_STRING_SIZE 10
#define BMP_HEADER_SIZE 54
#define BMP_PREFIX_SIZE 7

#include "error.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>

///
/// @brief This function retrievs input from the user as a char* and allocates it on the heap.
///
/// @returns The user input.
//
char* getInput();

///
/// @brief This function prints a short description description of the program
/// and all available options and their effects.
///
void printHelpMessage();

/// 
/// @brief This function checks wheather the given command line parameters are valid.
/// 
/// @param parameters An array of char pointers that contains the command 
/// line parameters starting from the third command line parameter.
/// @param count The number of elements in the parameters array. 
///
/// @return INVALID_USAGE if the number of arguments is less then 3,
///         INVALID_FILE if the given file paths do not point to files with the .bmp file extension, otherwise SUCCESS.
ErrorCode checkParameters(char* parameters[], const size_t count);

/// 
/// @brief This function checks if the given message is valid for encoding into the .bmp file.
/// 
/// @param user_input The message to be encoded.
/// 
/// @return False if the message length is 0 or greater than the maximum message length possible for the bitmap file.
/// 
bool isUserInputValid(char* user_input);

#endif // DEFINITIONS_H
