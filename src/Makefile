CC=mpicxx -cxx=g++ -std=c++0x -Wall -O3
a.out : main.o gate.o mymath.o mympi.o person.o playground.o reading.o
	$(CC) -o a.out main.o gate.o mymath.o mympi.o person.o playground.o reading.o
main.o : main.cpp
	$(CC) -o main.o -c main.cpp
gate.o : gate.cpp
	$(CC) -o gate.o -c gate.cpp
mymath.o : mymath.cpp
	$(CC) -o mymath.o -c mymath.cpp
mympi.o : mympi.cpp
	$(CC) -o mympi.o -c mympi.cpp
person.o : person.cpp
	$(CC) -o person.o -c person.cpp
playground.o : playground.cpp
	$(CC) -o playground.o -c playground.cpp
reading.o : reading.cpp
	$(CC) -o reading.o -c reading.cpp
clean : 
	rm *.o a.out

