#ifndef TRABALHO_PRATICO_SO_SRC_HEADERS_PERSONAGEM_H
#define TRABALHO_PRATICO_SO_SRC_HEADERS_PERSONAGEM_H

#include <iostream>

using namespace std;

class Personagem {
    public:
        Personagem(); // Construtor vazio
        Personagem(string name, int code); // Construtor com os parâmetros

        string getNome(); // Obtém o nome do personagem
        int getCodigo(); // Obtém o código do personagem

    private:
        string nome; // Nome do personagem
        int codigo; // Código do personagem
};

#endif //TRABALHO_PRATICO_SO_SRC_HEADERS_PERSONAGEM_H