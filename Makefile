CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -pedantic -O2
LDLIBS  = -lm
OBJS    = main.o artemis.o interfaz.o validaciones.o
TARGET  = artemis

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o:         main.c artemis.h interfaz.h validaciones.h
artemis.o:      artemis.c artemis.h
interfaz.o:     interfaz.c interfaz.h artemis.h validaciones.h
validaciones.o: validaciones.c validaciones.h

test: $(TARGET)
	./$(TARGET) --test

clean:
	rm -f $(OBJS) $(TARGET) reporte_artemis.txt

.PHONY: all test clean
