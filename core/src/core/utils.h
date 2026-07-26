#ifndef UTILS_H
#define UTILS_H

#include "core/stringdef.h"

#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_RESET "\x1b[0m"

/**
* @brief Checks if a file exists
* @param file The path of the file
* @return true if file exists, false otherwise
*/
bool file_exists(bstr file);


/**
* @brief Write the contents to a file
* @param output_path Contents are written to a file at this path
* @param str The string containing the contents to be written
* @return -1 if it failed to write to the file else returns 0
*/
int write_out(bstr output_path, bstr str);

#endif
