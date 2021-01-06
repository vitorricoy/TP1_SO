CC=g++
CFLAGS=-std=c++11 -Wall -Wextra
EXEC=./tp1

$(EXEC): main.cpp forno.o personagem.o raj.o
	$(CC) $(CFLAGS) main.cpp forno.o personagem.o raj.o -o $(EXEC)

personagem.o: headers/personagem.cpp
	$(CC) $(CFLAGS) -c headers/personagem.cpp -o personagem.o

forno.o: headers/forno.cpp personagem.o
	$(CC) $(CFLAGS) -c headers/forno.cpp -o forno.o

raj.o: headers/raj.cpp
	$(CC) $(CFLAGS) -c headers/raj.cpp -o raj.o

clean:
	rm -rf *.o
