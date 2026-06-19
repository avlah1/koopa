#ifndef SHELLTYPES_H
#define SHELLTYPES_H

#include <stdbool.h>

// Custom enumeration that allows for ShellLoop to terminate or continue iteration.
// SHELL_CONTINUE should be returned to ShellLoop if a process or a built-in function
//   terminated at all, whether successfully or with failure.
// SHELL_EXIT should be returned to ShellLoop if the user specifies EOF (Ctrl+D) or "exit".
typedef enum {
    SHELL_CONTINUE,
    SHELL_EXIT
} ShellStatus;

// Custom enumeration to track quote-parsing state while scanning a token character by character.
// QUOTE_NONE indicates the scanner is not currently inside a quoted section.
// QUOTE_SINGLE indicates the scanner is inside a single-quoted where all characters are treated as literal.
// QUOTE_DOUBLE indicates the scanner is inside a double-quoted where all characters are treated as literal.
typedef enum {
  QUOTE_NONE,
  QUOTE_SINGLE,
  QUOTE_DOUBLE
} QuoteState;

// Custom enumeration for ReadLine to communicate why reading stopped, since a return value of
// false alone cannot distinguish a normal end-of-input condition from a genuine failure.
// READ_OK is to be used when a full line was read successfully.
// READ_EOF is to be used when end-of-file (e.g. Ctrl+D) was reached, which is a normal,
//   expected way for input to end and should not be treated as an error.
// READ_SYSTEM_ERROR is to be used if a system failure occurs while reading, eg malloc/realloc fail.
typedef enum {
  READ_OK,
  READ_EOF,
  READ_SYSTEM_ERROR
} ReadResult;

// Custom enumeration for parsing functions that could result in more than two return values.
// PARSE_BAD_INPUT is to be used if the parser receives bad input, eg unbalanced quotes
// PARSE_SYSTEM_ERROR is to be used if a system failure occurs during parsing, eg malloc fail
// PARSE_OK is to be used when parsing is successful.
typedef enum {
  PARSE_OK,
  PARSE_BAD_INPUT,
  PARSE_SYSTEM_ERROR
} ParseResult;

// Represents a single parsed command, ready for execution.
// args is a NULL-terminated array of argument strings (args[0] is the program name),
//   suitable for passing directly to execvp.
// input_file is the filename to redirect stdin from (via '<'), or NULL if no input
//   redirection was specified.
// output_file is the filename to redirect stdout to (via '>' or '>>'), or NULL if no
//   output redirection was specified.
// num_args is the number of elements in args, not including the trailing NULL terminator.
// append indicates whether output_file should be opened in append mode ('>>') rather
//   than truncate mode ('>'). Only meaningful if output_file is non-NULL.
typedef struct {
  char** args;
  char* input_file;
  char* output_file;
  int num_args;
  bool append;
} Command;

#endif