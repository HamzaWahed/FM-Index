CC = gcc

TARGET = mem
SRCS = mem.c fm_index.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJS)
