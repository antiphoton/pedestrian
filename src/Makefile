CC=mpicxx -cxx=g++ -std=c++0x -Wall -O3
a.out : main.o gravity.o frame.o gate.o mymath.o mympi.o mytime.o person.o playground.o reading.o trajectory.o
	$(CC) -o a.out main.o frame.o gravity.o gate.o mymath.o mympi.o mytime.o person.o playground.o reading.o trajectory.o
main.o : main.cpp
	$(CC) -o main.o -c main.cpp
frame.o : frame.cpp
	$(CC) -o frame.o -c frame.cpp
gravity.o : gravity.cpp
	$(CC) -o gravity.o -c gravity.cpp
gate.o : gate.cpp
	$(CC) -o gate.o -c gate.cpp
mymath.o : mymath.cpp
	$(CC) -o mymath.o -c mymath.cpp
mympi.o : mympi.cpp
	$(CC) -o mympi.o -c mympi.cpp
mytime.o : mytime.cpp
	$(CC) -o mytime.o -c mytime.cpp
person.o : person.cpp
	$(CC) -o person.o -c person.cpp
playground.o : playground.cpp
	$(CC) -o playground.o -c playground.cpp
reading.o : reading.cpp
	$(CC) -o reading.o -c reading.cpp
trajectory.o : trajectory.cpp
	$(CC) -o trajectory.o -c trajectory.cpp
clean : 
	rm *.o a.out

