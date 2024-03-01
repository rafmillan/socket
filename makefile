server: server.c
	gcc-13 -lpthread server.c -o server

client: client.c
	gcc-13 -lpthread client.c -o client

all: server client

clean:
	rm -f server client

