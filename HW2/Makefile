CC := g++
exe := pi.out
obj := pi.o

all : $(obj)
	$(CC) -o $(exe) $(obj) -pthread

%.o : %.c
	$(CC) -c $^ -o $@

clean:
	rm -f $(exe) *.o