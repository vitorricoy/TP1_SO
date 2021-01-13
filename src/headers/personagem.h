#ifndef TRABALHO_PRATICO_SO_SRC_HEADERS_PERSONAGEM_H
#define TRABALHO_PRATICO_SO_SRC_HEADERS_PERSONAGEM_H

#include <iostream>

using namespace std;

class Personagem {
    public:
        Personagem();
        Personagem(string name, int code);

        string getNome();
        int getCodigo();

    private:
        string nome;
        int codigo;
};

#endif //TRABALHO_PRATICO_SO_SRC_HEADERS_PERSONAGEM_H