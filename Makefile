CC=g++
CFLAGS=-std=c++11 -Wall -Wextra -Wno-unused-parameter
EXEC=tp1

$(EXEC): src/main.cpp forno.o personagem.o info_personagem_fila.o
	$(CC) $(CFLAGS) src/main.cpp -pthread forno.o info_personagem_fila.o personagem.o -o $(EXEC)

info_personagem_fila.o: src/headers/info_personagem_fila.cpp
	$(CC) $(CFLAGS) -c src/headers/info_personagem_fila.cpp -o info_personagem_fila.o

personagem.o: src/headers/personagem.cpp
	$(CC) $(CFLAGS) -c src/headers/personagem.cpp -o personagem.o

forno.o: src/headers/forno.cpp personagem.o info_personagem_fila.o
	$(CC) $(CFLAGS) -c src/headers/forno.cpp -o forno.o

clean:
	rm -rf *.o
