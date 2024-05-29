CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -std=c99

INCLUDE= -|cli-lib/include

SRC_DIR=cli-lib/src
SRCS = main.c keyboard.c

OBJS = $(SRCS:.c=.o)

TARGET = breakout

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
