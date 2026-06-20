#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_
#include <stdbool.h>

#include "ShellTypes.h"

// Reads bytes from stdin until a newline character or EOF is reached, converting the bytes to a C-String. 
// The bytes of the string are placed in a newly heap-allocated buffer and pointer to the buffer
// is returned through "line_ret". If this function returns READ_OK, the caller assumes responsibility 
// for the heap-allocated C-string. 
// Arguments:
// - line_ret: a return parameter that will store a pointer to the heap-allocated C-string.
//   Only valid if the function returns READ_OK.
// Returns:
// - READ_OK on success.
// - READ_EOF when EOF is reached (eg Ctrl + D)
// - READ_SYSTEM_ERROR when a system failure occurs while reading bytes into memory (eg malloc/realloc fail).
ReadResult ReadLine(char** line_ret);

// Parses tokenized argument list into a Command struct, extracting and removing 
// any I/O redirection operators and their associated filenames from "args" so that the 
// resulting cmd->args contains only program arguments to be passed to execvp. Supports multiple
// redirections within the same line.
// Arguments:
// - args: a NULL-terminated array of token pointers. This array is modified in-place (redirection
//   operators and filenames removed by shifting later elements left). The caller retains ownership 
//   of it regardless of the return value.
// - num_args: the number of token in "args", not including the NULL terminator. 
// - cmd_ret: a return parameter that will store a pointer to a newly heap-allocated Command struct.
//   Only valid when the return value is PARSE_OK, the caller owns this memory and is responsible for freeing it.
// Returns:
// - PARSE_OK on success.
// - PARSE_BAD_INPUT when a redirection operator is found with no filename following it (eg the operator is the last token).
// - PARSE_SYSTEM_ERROR when a system failure occurs while building the Command struct (eg calloc fail).
ParseResult ParseLine(char** args, int num_args, Command** cmd_ret);

// Tokenizes C-string "line" in-place. Pointers to the individual tokens
// are placed in contiguous memory on the heap, and a pointer to the first
// arg pointer is returned in the output parameter "args_ptr_ret" and the number of
// arguments that were tokenized is returned in the output parameter "num_args_ret".
// At args_ptr_ret[num_args_ret], NULL is placed, denoting the end of the args list.
// If the return value is PARSE_OK, the caller will own the heap allocated memory pointed
// to by "args_ptr_ret", but this function never assumes responsibility for "line".
// Arguments:
// - line: the C-string to tokenize in-place. The caller retains ownership and is
//   responsible for freeing it.
// - args_ptr_ret: a return parameter that will store a pointer to the heap-allocated
//   array of token pointers (each pointing into "line"). Only valid when the return
//   value is PARSE_OK.
// - num_args_ret: a return parameter that will store the number of tokens found,
//   not including the trailing NULL terminator. Only valid when the return value
//   is PARSE_OK.
// Returns:
// - PARSE_OK on success.
// - PARSE_BAD_INPUT when the C-string at "line" is malformed (e.g. unbalanced quotes).
// - PARSE_SYSTEM_ERROR when a system failure occurs during parsing (e.g. malloc fail).
ParseResult TokenizeLine(char* line, char*** args_ptr_ret, int* num_args_ret);

#endif  // INPUT_HANDLER_H_