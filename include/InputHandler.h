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

// Custom enumeration for parsing functions that could result in more than two return values.
// PARSE_BAD_INPUT is to be used if the parser receives bad input, eg unbalanced quotes
// PARSE_SYSTEM_ERROR is to be used if a system failure occurs during parsing, eg malloc fail
// PARSE_OK is to be used when parsing is successful.
typedef enum {
  PARSE_OK,
  PARSE_BAD_INPUT,
  PARSE_SYSTEM_ERROR
} ParseResult;

// Tokenizes C-string "line" in-place. Pointers to the individual tokens 
// are placed in contiguous memory on the heap, and a pointer to the first 
// arg pointer is returned in the output paramter "args_ptr_ret" and the number of
// arguments that were tokenized is returned in the output parameter "num_args_ret".
// At args_ptr_ret[num_args_ret], NULL is placed, denoting the end of the args list.
// If the return value is PARSE_OK, the caller will own the heap allocated memory pointed
// to by "args_ptr_ret", but this function never assums responsibility for "line"
// This function returns:
// - PARSE_OK on success
// - PARSE_BAD_INPUT when the C-string at "line" is unexpected (eg, unbalanced quotes)
// - PARSE_SYSTEM_ERROR when a system failure occurs during parsing (eg, malloc fail)
ParseResult ParseLine(char* line, char*** args_ptr_ret, int* num_args_ret);

typedef enum {
  QUOTE_NONE,
  QUOTE_SINGLE,
  QUOTE_DOUBLE
} QuoteState;

// Splits the C-string "str" at delimeters "delimeters", one at a time. 
// Tokens are split with a '\0'.  If successful,
// a pointer to a newly formed token is returned via the return parameter "token_ret".
// A first call to this function should place the actual string to be tokenized at str.
// For subsequent calls, NULL should be passed as "str".
// This function returns:
// - PARSE_OK when the end of the string "str" is reached (eg, a null character)
// - PARSE_BAD_INPUT when a portion of "str" is unexpected (eg, unbalanced quotes)
ParseResult GetToken(char* str, char** token_ret, char* delimiters);

#endif  // INPUT_HANDLER_H_