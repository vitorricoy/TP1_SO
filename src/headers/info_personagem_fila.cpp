#include <iostream>
#include "info_personagem_fila.h"
#include "constantes.h"

using namespace std;

InfoPersonagemFila::InfoPersonagemFila() {
    this->nivelPrioridade = 0;
    this->tempoChegada = 0;
    this->presenteNaFila = false;
    this->posicaoFila = -1;
}

void InfoPersonagemFila::setPrioridade(int prioridade) {
    this->nivelPrioridade = prioridade;
}

void InfoPersonagemFila::setPosicao(int posicao) {
    this->posicaoFila = posicao;
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

int InfoPersonagemFila::getPosicao() {
    return this->posicaoFila;
}

bool InfoPersonagemFila::temMaisPrioridade(InfoPersonagemFila comparacao) {
    if(!this->presenteNaFila || comparacao.getPrioridade() > this->getPrioridade()) {
        return false;
    }
    
    if(!comparacao.estaNaFila() || this->getPrioridade() > comparacao.getPrioridade()) {
        return true;
    }

    if(this->getPosicao() >= 6 && comparacao.getPosicao() <= 5) {
        return false;
    }

    if(this->getPosicao() <= 5 && comparacao.getPosicao() >= 6) {
        return true;
    }

    if(this->getPosicao() >= 6 && comparacao.getPosicao() >= 6) {
        return this->getPosicao() < comparacao.getPosicao();
    }

    if(this->getPosicao() <= 5) {
        int idtCasal = this->getPosicao()/2;
        int idtCasalComparacao = comparacao.getPosicao()/2;
        if(idtCasal == idtCasalComparacao) {
            return this->getTempoChegada() < comparacao.getTempoChegada();
        } else {
            if((idtCasal+1)%Constantes::NUMERO_CASAIS == idtCasalComparacao) {
                return true;
            } else {
                return false;
            }
        }
    }

    return false;

}