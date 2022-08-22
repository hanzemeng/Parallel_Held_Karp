build: main_static.c main_dynamic.c
	gcc -o main_s -fopenmp -lm main_static.c
	gcc -o main_d -fopenmp -lm main_dynamic.c

main_s: main_static.c
	gcc -o main_s -fopenmp -lm main_static.c
main_d: main_dynamic.c
	gcc -o main_d -fopenmp -lm main_dynamic.c

run:
	./main_s points.txt 2
	./main_d points.txt 2

clean:
	rm -f main_s main_d