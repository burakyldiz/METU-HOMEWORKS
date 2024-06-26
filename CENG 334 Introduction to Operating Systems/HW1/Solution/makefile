
CC=gcc


CFLAGS=-I. -Wall

TARGET=eshell


OBJS=main.o parser.o

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

main.o: main.c parser.h
	$(CC) $(CFLAGS) -c main.c


parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c


clean:
	rm -f $(OBJS) $(TARGET)


.PHONY: clean
