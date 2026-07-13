# Koopa

A Unix shell written from scratch in C, built to deepen my understanding of systems programming concepts like process management, I/O redirection, and parsing.

## Features:
- Execute standard Unix commands
- Built-in commands include cd, status, and exit 
- Conditional operator support
- I/O redirection using <, >, and >>
- Single and double quoted input
- Robust error handling

## Features coming soon
- Piping
- Background execution

## Building/Installation
- GCC (14+) and Make required, with C23 support
- To build: make
- To run: ./Koopa

## Known Limitations
- I/O redirection on middle commands in a pipeline is not supported
- Colored output to distinguish directories from files, grepped text, etc.