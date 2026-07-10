#ifndef KOOPA_H_
#define KOOPA_H_

#include <stdbool.h>

#include "InputHandler.h"
#include "Execute.h"
#include "Colors.h"
#include "ShellTypes.h"
#include "CommandChain.h"

// TODO: function spec, holding off on it for now since this function keeps changing.
bool ShellLoop();

// Retrieves the current working directory into a heap-allocated buffer, doubling
// the buffer size and retrying if the path exceeds initial buffer size. The caller
// owns the returned string and is responsible for freeing it. Returns false
// if allocation fails.
bool GetCurrentDir(char** path_ret);

#endif  // KOOPA_H_