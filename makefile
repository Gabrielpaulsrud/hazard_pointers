CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OBJ = lock_free_stack.o experiment.o
TARGET = experiment

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

lock_free_stack.o: lock_free_stack.c lock_free_stack.h
	$(CC) $(CFLAGS) -c lock_free_stack.c

experiment.o: experiment.c lock_free_stack.h
	$(CC) $(CFLAGS) -c experiment.c

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)