all: solver

solver: solver.o
	gcc -o solver solver.o

solver.o: mazeSolver.c
	gcc -c mazeSolver.c -o solver.o

clean:
	rm -f solver.o

fclean: clean
	rm -f solver

re: fclean all