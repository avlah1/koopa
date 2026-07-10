#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_
#include <stdbool.h>

#include "ShellTypes.h"
#include "CommandChain.h"
#include "Colors.h"

// Reads a line from stdin into a heap-allocated buffer, growing buffer as needed. 
// The caller owns the returned buffer and is responsible for freeing it. line_ret 
// is only valid when the return value is READ_OK.
// Returns READ_OF if end of file is reached (eg Ctrl + D), or READ_SYSTEM_ERROR if 
// a system error occurs (eg malloc/realloc fail).
ReadResult ReadLine(char** line_ret);

// Parses a flat token array into a CommandChain struct by splitting on conditional operators
// (&& || ;) and calling ParseCommand on each resulting segment. Each segment becomes a Command struct 
// in the chain with its cond_op field set based on operator that preceded it. The caller owns the returned
// chain and is responsible for freeing it via CommandChain_Free. chain_ret is only valid 
// when the return value is PARSE_OK.
ParseResult ParseCommandChain(char** tokens, int num_tokens, CommandChain** chain_ret);

// Tokenizes a line in-place by calling GetToken repeatedly, collecting pointers to each token
// into a heap-allocated array. The array is NULL-terminated and its pointer is returned via
// args_ptr_ret. The caller owns the returned array that stores the pointers. This 
// function never assumes responsibility for the strings stored in "line".
// Returns PARSE_BAD_INPUT if a token contains unbalanced quotes, or PARSE_SYSTEM_ERROR
// if an allocation failure occurs.
ParseResult TokenizeLine(char* line, char*** args_ptr_ret, int* num_args_ret);

#endif  // INPUT_HANDLER_H_