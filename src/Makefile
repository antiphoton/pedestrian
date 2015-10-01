CC=mpicxx -Wall -O3
a.out : main.o mympi.o
	$(CC) -o a.out -lgif -ljpeg main.o mympi.o
main.o : main.cpp
	$(CC) -o main.o -c main.cpp
mympi.o : mympi.cpp
	$(CC) -o mympi.o -c mympi.cpp
clean : 
	rm *.o a.out

