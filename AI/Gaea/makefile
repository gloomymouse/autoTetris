autoTetris_client: vars.o msg.o ai.o tetris.o client.c
	@gcc -g vars.o msg.o ai.o tetris.o client.c -o autoTetris_client -lpthread
tetris.o:
	@gcc -o tetris.o -c tetris.c
ai.o:
	@gcc -o ai.o -c ai.c
msg.o:
	@gcc -o msg.o -c msg.c
vars.o:
	@gcc -o vars.o -c vars.c
clean:
	@rm -f *.o
