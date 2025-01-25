CC = gcc
CFLAGS = -Wall -Wextra -pedantic
TARGET = imcsh

all: $(TARGET)

$(TARGET): imcsh.c
	$(CC) $(CFLAGS) -o $(TARGET) imcsh.c

clean:
	rm -f $(TARGET)
