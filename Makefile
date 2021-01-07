CC=g++
CFLAGS=-std=c++11 -Wall -Wextra
EXEC=tp1

$(EXEC): src/main.cpp forno.o personagem.o raj.o
	$(CC) $(CFLAGS) src/main.cpp forno.o personagem.o raj.o -o $(EXEC)

personagem.o: src/headers/personagem.cpp
	$(CC) $(CFLAGS) -c src/headers/personagem.cpp -o personagem.o

forno.o: src/headers/forno.cpp personagem.o
	$(CC) $(CFLAGS) -c src/headers/forno.cpp -o forno.o

raj.o: src/headers/raj.cpp
	$(CC) $(CFLAGS) -c src/headers/raj.cpp -o raj.o

clean:
	rm -rf *.o
