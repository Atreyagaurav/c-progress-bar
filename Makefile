CC = gcc
CFLAGS = -Wall
DEPS = c_progress.h
OBJ = c_progress.o demo.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: demo lib

demo: c_progress.o demo.o
	gcc $(CFLAGS) -o $@ $^

lib: c_progress.o
	swig -python c_progress.i
	python setup.py build_ext --inplace

pyinstall: lib
	python setup.py install --user

clean:
	rm *.o rm *.so c_progress c_progress.py c_progress_wrap.c
