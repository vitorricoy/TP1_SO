#include <iostream>
#include "personagem.h"

using namespace std;

Personagem::Personagem(string name, int code) {
    nome = name;
    codigo = code;
}

Personagem::Personagem() : Personagem("", -1) {}

string Personagem::getNome() {
    return nome;
}

int Personagem::getCodigo() {
    return codigo;
}