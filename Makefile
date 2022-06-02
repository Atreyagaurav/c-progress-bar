CC = gcc
CFLAGS = -Wall
DEPS = progress.h
OBJ = progress.o 

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: progress lib

progress: progress.o
	gcc $(CFLAGS) -o $@ $^

lib: progress.o
	swig -python progress.i
	python setup.py build_ext --inplace

clean:
	rm *.so progress
