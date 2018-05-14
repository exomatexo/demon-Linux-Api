all: 
	gcc main.c function.c -o demon
clean:
	rm -f *.o*
	rm demon
