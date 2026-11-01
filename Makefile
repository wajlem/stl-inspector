CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 -O2
LDFLAGS = -lm

TARGET = stl-inspector
SRCS = src/stl-inspector.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
