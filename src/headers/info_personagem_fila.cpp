#include <iostream>
#include "info_personagem_fila.h"

using namespace std;

InfoPersonagemFila::InfoPersonagemFila() {
    this->nivelPrioridade = 0;
    this->tempoChegada = 0;
    this->presenteNaFila = false;
}

void InfoPersonagemFila::setPrioridade(int prioridade) {
    this->nivelPrioridade = prioridade;
}

void InfoPersonagemFila::entrarNaFila(int tempoChegada) {
    this->presenteNaFila = true;
    this->tempoChegada = tempoChegada;
}

void InfoPersonagemFila::sairDaFila() {
    this->presenteNaFila = false;
}

bool InfoPersonagemFila::estaNaFila() {
    return this->presenteNaFila;
}

int InfoPersonagemFila::getTempoChegada() {
    return this->tempoChegada;
}

int InfoPersonagemFila::getPrioridade() {
    return this->nivelPrioridade;
}