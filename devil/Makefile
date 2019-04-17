TARGET = test

CC = gcc

CFLAGS = -c -v

VPATH = src
SRC = test.c
OBJ = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) -o $@ $(OBJ)

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) $<
clean:
	rm -rf $(TARGET) $(TARGET).exe
	rm -rf *.o *.obj
run:
	./$(TARGET)
