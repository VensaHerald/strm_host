strm_host: main.o
	gcc -Wall -o strm_host main.o -l"Ws2_32"

main.o: main.c headers.h
	gcc -g3 -Wall -c main.c headers.h

clean:
	del *.o *.exe *.h.gch
	