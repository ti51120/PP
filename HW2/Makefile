pi.out: pi.o
	g++ -g pi.o -o pi.out 
	-rm *.o
pi.o: pi.cpp
	g++ -c -g -std=c++11 pi.cpp -o pi.o
clean:
	-rm *out