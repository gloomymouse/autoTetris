autoTetris: vars.o tetris.o server.c
	@gcc -g vars.o tetris.o server.c -o autoTetris -lpthread
tetris.o:
	@gcc -o tetris.o -c tetris.c
vars.o:
	@gcc -o vars.o -c vars.c
clean:
	@rm -f *.o

