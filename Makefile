CC = g++
CFLAGS = -Wall -g

all:	ex1 ex2

sim.o : sim.cc sim.hh
	${CC} ${CFLAGS} -c sim.cc

ex1.o : ex1.cc
	${CC} ${CFLAGS} -c ex1.cc

ex1 : sim.o ex1.o
	${CC} ${CFLAGS} sim.o ex1.o -o ex1

ex2.o : ex2.cc
	${CC} ${CFLAGS} -c ex2.cc

ex2 : sim.o ex2.o
	${CC} ${CFLAGS} sim.o ex2.o -o ex2

run:
	./ex1
	./ex2

clean:
	rm -f ex1 ex1.o sim.o ex2 ex2.o
