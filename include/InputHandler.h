#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_
#include <stdbool.h>

#include "ShellTypes.h"

// Creates a C-string by reading byte by byte through stdin. On success, a pointer
// to the string is placed in "line_ptr_ret", which the caller owns and is
// responsible for freeing.
// Arguments:
// - line_ptr_ret: a return parameter that will store a pointer to
//   the allocated C-string. Only valid when the return value is READ_OK.
// Returns:
// - READ_OK if a line was read successfully.
// - READ_EOF if end-of-file (e.g. Ctrl+D) was reached before any line was completed;
//   this is a normal termination condition, not an error.
// - READ_SYSTEM_ERROR if a system failure occurred while reading (e.g. malloc/realloc fail).
ReadResult ReadLine(char** line_ptr_ret);


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
//   responsible for freeing it; this function only writes null terminators into it
//   and does not allocate or free it.
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