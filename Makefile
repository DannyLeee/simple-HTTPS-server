all: server

args = -lssl -lcrypto
server: server.c initial.c initial.h
	[ -f server ] && rm server || echo "no file name server"
	gcc -o server server.c initial.c $(args)