all: cli_i.o srv_i.o
	gcc -o cli_i cli_i.o
	gcc -o srv_i srv_i.o
clean:
	rm srv_i cli_i
	rm *.o
	rm *~
