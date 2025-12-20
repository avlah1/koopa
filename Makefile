all: koopa

koopa: koopa.o input_handler.o builtins.o execute.o redirect.o
	gcc -g -Wall -o koopa koopa.o input_handler.o builtins.o execute.o redirect.o

koopa.o: src/koopa.c
	gcc -g -Wall -Iinclude -c src/koopa.c -o koopa.o

input_handler.o: src/input_handler.c
	gcc -g -Wall -Iinclude -c src/input_handler.c -o input_handler.o

redirect.o: src/redirect.c
	gcc -g -Wall -Iinclude -c src/redirect.c -o redirect.o

execute.o: src/execute.c builtins.o input_handler.o redirect.o
	gcc -g -Wall -Iinclude -c src/execute.c -o execute.o

builtins.o: src/builtins.c
	gcc -g -Wall -Iinclude -c src/builtins.c -o builtins.o
