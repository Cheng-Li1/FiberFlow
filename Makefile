# Makefile for fiberflow
FF_DIR := ./

# Compiler
CC = clang

TARGET := aarch64-none-elf

# Compiler flags
CFLAGS = -Wall \
         -Wextra \
		 -Werror \
		 -target $(TARGET) \
		 -ffreestanding \
		 -g \
		 -O0 \
		 -mstrict-align \

# Source files
SRCS = FiberFlow.c

# Object files (generated from source files)
OBJS = $(SRCS:.c=.o)

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

STATIC_LIB = libfiberflow.a

# Rule to link the target executable
$(STATIC_LIB): $(OBJS)
	ar rcs $@ $(OBJS)

# Phony targets (not files)
.PHONY: all clean