CC = gcc
CFLAGS = -Wall
DEPS = c_progress.h
OBJ = c_progress.o 

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: c_progress lib

c_progress: c_progress.o
	gcc $(CFLAGS) -o $@ $^

lib: c_progress.o
	swig -python c_progress.i
	python setup.py build_ext --inplace

pyinstall: lib
	python setup.py install --user

clean:
	rm *.o rm *.so c_progress c_progress.py c_progress_wrap.c
