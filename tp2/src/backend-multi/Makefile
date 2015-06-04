CC = g++
CFLAGS = -g -Wall -Werror
LDFLAGS = -pthread

BINARIES = backend

all: $(BINARIES)

backend: Backend_multi.cpp Backend_multi.h Enviar_recibir.o RWLock.o RWLock.h Enviar_recibir.h Encabezado.h Casillero.h
	$(CC) $(LDFLAGS) $(CFLAGS) Backend_multi.cpp Enviar_recibir.o RWLock.o -o backend

Enviar_recibir.o: Enviar_recibir.cpp Enviar_recibir.h Encabezado.h
	$(CC) $(CFLAGS) -c Enviar_recibir.cpp

RWLock.o: RWLock.cpp 
	$(CC) $(CFLAGS) RWLock.cpp -c

clean:
	rm -f *.o
	rm -f $(BINARIES)
