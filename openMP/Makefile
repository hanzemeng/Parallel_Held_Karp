FLAGS = -Wall -lm -fopenmp

build: main_dynamic.c main_static.c
	gcc-12 main_dynamic.c -o main_dynamic ${FLAGS}
	gcc-12 main_static.c -o main_static ${FLAGS}

main_dynamic: main_dynamic.c
	gcc main_dynamic.c -o main_dynamic ${FLAGS}
main_static: main_static.c
	gcc main_static.c -o main_static ${FLAGS}

run:
	./main_dynamic points.txt 4
	./main_static points.txt 4

clean:
	rm -f main_static main_dynamic
