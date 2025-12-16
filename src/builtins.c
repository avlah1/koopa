#include <stdio.h>


int kpa_exit(char**);

char* builtins_strs[] = {
	"exit"
};

int num_builtins() {
	return sizeof(builtins_strs) / sizeof(char*);
}

int (*built_ins[])(char**) = {
	&kpa_exit
};

int kpa_exit(char**) {
	return 1;
}

