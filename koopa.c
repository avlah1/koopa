#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 128

char* read_line() {
	int buffer_size = BUFSIZE;
	char* buffer = malloc(sizeof(char) * buffer_size);
	int position;
	int c;

	if (buffer == NULL) {
		printf("koopa line reader error");
		return NULL;
	}
	
	while (1) {
		c = getchar();
		if (c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}
		position++;

		if (position >= buffer_size) {
			buffer_size += BUFSIZE;
			buffer = realloc(buffer, buffer_size);
			if (buffer == NULL) {
				printf("koopa line reader error");
				return NULL;
			}
		}
	}


}


void shell_loop() {
	char* line;
        
	printf("kpa>>");
	line = read_line();
	printf("%s\n", line);
	free(line);
}


int main() {

	shell_loop();
	return 0;

}
