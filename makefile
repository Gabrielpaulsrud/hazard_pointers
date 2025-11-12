CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2 -pthread
TARGET  = experiment

# Pick stack impl: plain (default) or numbered
#STACK ?= plain
STACK ?= tagged
STACK_SRC = lock_free_stack_$(STACK).c

OBJS = $(TARGET).o $(STACK_SRC:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# experiment depends on the public API
$(TARGET).o: $(TARGET).c lock_free_stack.h
	$(CC) $(CFLAGS) -c $< -o $@

# either stack implementation also depends on the header
lock_free_stack_%.o: lock_free_stack_%.c lock_free_stack.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: debug dbg

# Build with debug flags
debug: CFLAGS += -O0 -g -fno-omit-frame-pointer
debug: clean $(TARGET)

# Build (debug) then run LLDB
dbg: debug
	lldb ./$(TARGET)