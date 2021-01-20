#include <iostream>
#include "personagem.h"

using namespace std;

Personagem::Personagem(string name, int code) {
    // Salva os valores recebidos por argumento
    nome = name;
    codigo = code;
}

Personagem::Personagem() : Personagem("", -1) {} // Construtor vazio cria um personagem inválido

string Personagem::getNome() {
    return nome;
}

int Personagem::getCodigo() {
    return codigo;
}