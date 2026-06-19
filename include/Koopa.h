#ifndef KOOPA_H_
#define KOOPA_H_

#include <stdbool.h>

#include "InputHandler.h"
#include "Execute.h"
#include "Colors.h"
#include "ShellTypes.h"


// TODO: write function spec
bool ShellLoop();

// TODO: write function spec
bool GetCurrentDir(char** path_ptr_ret);

#endif