CC = gcc
CFLAGS = -g -Wall -std=c11 -D_POSIX_C_SOURCE=200809L

TARGET = Koopa
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:src/%.c=obj/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj
	
clean:
	rm -f $(OBJS) $(TARGET) *.txt