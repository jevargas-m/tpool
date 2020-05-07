# Define variable CC to be the compiler we want to use
CC = gcc

# Define CFLAGS for the flags we will want to use with clang
CFLAGS = -g -Wall -ansi -pthread

# Output executable
TARGET = my_test

# Output object files dependencies for linking
TARGET_DEPENDENCIES = my_test.o tpool.o

# ensure these targets are executed even if there is a file named like that
.PHONY: clean all

# If no arguments are passed to make, it will attempt the 'all' target
default: all
all: $(TARGET) 

# TARGET_DEPENDENCIES
# $< first dependency,  $^ all dependencies,   $@ the file LHS of ':'

my_test.o: my_test.c 
	$(CC) -c $(CFLAGS) $< -o $@

tpool.o: ../tpool.c ../tpool.h 
	$(CC) -c $(CFLAGS) $< -o $@

# TARGET linking
$(TARGET): $(TARGET_DEPENDENCIES)
	$(CC) $(CFLAGS) $^ -o $@

# $(RM) is the platform agnostic way to delete a file (here rm -f)
clean:
	$(RM) $(TARGET)
	$(RM) $(TARGET_DEPENDENCIES)