build: main_static.c main_dynamic.c
	gcc -o main_static -fopenmp -lm main_static.c
	gcc -o main_dynamic -fopenmp -lm main_dynamic.c

main_static: main_static.c
	gcc -o main_static -fopenmp -lm main_static.c
main_dynamic: main_dynamic.c
	gcc -o main_dynamic -fopenmp -lm main_dynamic.c

run:
	./main_static points.txt 2
	./main_dynamic points.txt 2

clean:
	rm -f main_static main_dynamic
