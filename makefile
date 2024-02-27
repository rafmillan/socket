server: server.c
	gcc -lpthread server.c -o server

client: client.c
	gcc -lpthread client.c -o client

all: server client

clean:
	rm -f server client

