#include <stdio.h>
#include <unistd.h>

int kpa_exit(char**);
int kpa_cd(char**);

char* builtins_strs[] = {
	"exit",
	"cd"
};

int num_builtins() {
	return sizeof(builtins_strs) / sizeof(char*);
}

int (*built_ins[])(char**) = {
	&kpa_exit,
	&kpa_cd
};

int kpa_cd(char** args) {
	// Check for the directory in args[1]
	if (args[1] == NULL) {
		fprintf(stderr, "expected directory missing\n");
	} else {
		if (chdir(args[1]) != 0) {
				perror("koopa");
		}
	}

	return 1;
}

int kpa_exit(char**) {
	return 1;
}

