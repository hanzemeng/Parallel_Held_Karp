FLAGS = -Wall -lm -lpthread

main: main.c
	gcc main.c -o main ${FLAGS}

run:
	./main

clean:
	rm -f main