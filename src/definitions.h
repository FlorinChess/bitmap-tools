#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define INDEX_OFFSET 1
#define DEFAULT_STRING_SIZE 10
#define BMP_HEADER_SIZE 54
#define BMP_PREFIX_SIZE 7

#include "error.h"
#include <stdbool.h>
#include <stddef.h>

///
/// @brief This function retrievs input from the user as a char* and allocates it on the heap.
///
/// @returns The user input.
//
char* getInput();

/// @brief This function converts a numeric value into a 4-digit string representation.
///
/// @param length The number value representing the length of the message.  
/// @param buffer The buffer in which the resulting string is stored in.
///
void lengthToString(size_t length, char buffer[4]);

///
/// @brief This function reads a .bmp file and creates a layer of all the pixels in it converted to ASCII characters.
/// The new layer is added to the layer collection.
///
/// @param file The .bmp file.
/// @param layer_collection The layer collection
///
/// @returns The error code:
///   OUT_OF_MEMROY
///   SUCCESS
ErrorCode parseBinaryFile(char* file_path);


/// @brief This function returns the fully qualified file path of a given file without the file extension.
///
/// @param file_path The full file path containing the file extension.
/// @return The full file path without the extension.
///
char* getFilePathWithoutExtension(char* file_path);

///
/// @brief This function checks if a character represents a digit from 0 to 9.
///
/// @param character The character to check.
///
/// @return True if the parameter represents a digit from 0 to 9.
///
bool isCharNumber(char character);

int cp(const char* to, const char* from);

///
/// @brief This function prints a short description description of the program
/// and all available options and their effects.
///
void printHelpMessage();

/// @brief This function checks wheather the given command line parameters are valid.
/// 
/// @param parameters An array of char pointers that contains the command 
/// line parameters starting from the third command line parameter.
/// @param count The number of elements in the parameters array. 
///
/// @return INVALID_USAGE if the number of arguments is less then 3,
///         INVALID_FILE if the given file paths do not point to files with the .bmp file extension, otherwise SUCCESS.
ErrorCode checkParameters(char* parameters[], const size_t count);

bool isUserInputValid(char* user_input);

#endif // DEFINITIONS_H
