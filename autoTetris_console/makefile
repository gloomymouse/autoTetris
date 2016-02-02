autoTetris_console: vars.o msg.o tetris.o client.c
	@gcc -g vars.o msg.o tetris.o client.c -o autoTetris_console -lpthread -lcurses
tetris.o:
	@gcc -o tetris.o -c tetris.c
msg.o:
	@gcc -o msg.o -c msg.c
vars.o:
	@gcc -o vars.o -c vars.c
clean:
	@rm -f *.o
