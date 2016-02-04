autoTetris_server: vars.o msg.o tetris.o server.c
	@gcc -g vars.o msg.o tetris.o server.c -o autoTetris_server -lpthread
tetris.o:
	@gcc -o tetris.o -c tetris.c
msg.o:
	@gcc -o msg.o -c msg.c
vars.o:
	@gcc -o vars.o -c vars.c
clean:
	@rm -f *.o

