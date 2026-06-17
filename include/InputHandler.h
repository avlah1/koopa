#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_
#include <stdbool.h>

// Creates a C-string by reading byte by byte through stdin. On success, pointer
// to the string is placed in "line_ptr_ret", which the caller owns. 
// Arguments:
// - line_ptr_ret: a return parameter that will store a pointer to 
//   the allocated C-string
// Returns:
// - true if successful (eg there was no issue reading the bytes from 
//   stdin), false otherwise. 
bool ReadLine(char** line_ptr_ret);

typedef enum {
  PARSE_OK,
  PARSE_BAD_INPUT,
  PARSE_SYSTEM_ERROR
} ParseResult;

ParseResult ParseLine(char* line, char*** args_ptr_ret, int* num_args_ret);


ParseResult GetToken(char* str, char** token_ret, char* delimiters);

#endif  // INPUT_HANDLER_H_