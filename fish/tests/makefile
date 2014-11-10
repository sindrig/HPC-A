OPT = -O2

CC = mpicc
CFLAGS = -Wall -std=c99
LIBS = -lm

test: clean all
	mpirun -n 4 ./first.o

all: target
target: first

OBJS = first.c fish_model.c fish_utils.c net_model.c

first : $(OBJS)
	$(CC) -o first.o $(OBJS) $(OPT) $(LIBS) $(CFLAGS)

clean :
	rm -rf *.o *.o[0-9][0-9][0-9][0-9][0-9]
