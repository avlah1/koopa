// TODO: exit and cd

char* built_ins[] = {
	"exit"
};

// EXAMPLE: if only one built-in, that is ONE pointer, so 8 bytes. 8 / 8 = 1
int num_built_ins() {
	return sizeof(built_ins) / sizeof(char*);
}

int kpa_exit(char** args) {
	return 0;
}
