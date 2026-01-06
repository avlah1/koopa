#ifndef BUILTINS_H
#define BUITLINS_H

int num_builtins();
extern char* builtins_strs[];
extern int (*built_ins[])(char**);
int kpa_exit(char**);
int kpa_cd(char**);

#endif
