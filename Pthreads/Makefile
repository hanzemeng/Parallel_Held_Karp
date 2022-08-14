FLAGS = -Wall -lm -lpthread

build: main_cycle.c main_block.c
	gcc main_cycle.c -o main_cycle ${FLAGS}
	gcc main_block.c -o main_block ${FLAGS}

main_cycle: main_cycle.c
	gcc main_cycle.c -o main_cycle ${FLAGS}
main_block: main_block.c
	gcc main_block.c -o main_block ${FLAGS}

run:
	./main_cycle points.txt 7

clean:
	rm -f main_block main_cycle