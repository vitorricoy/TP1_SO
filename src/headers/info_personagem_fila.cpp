#include <iostream>
#include "info_personagem_fila.h"
#include "constantes.h"

using namespace std;

InfoPersonagemFila::InfoPersonagemFila() {
    this->nivelPrioridade = 0;
    this->tempoChegada = 0;
    this->presenteNaFila = false;
    this->codigoPersonagem = -1;
}

void InfoPersonagemFila::setPrioridade(int prioridade) {
    // if(prioridade != getPrioridade()) {
    //     cout << this->getCodigo() << " mudou para prioridade " << prioridade << endl;
    // }
    this->nivelPrioridade = prioridade;
}

void InfoPersonagemFila::setCodigo(int codigo) {
    this->codigoPersonagem = codigo;
}

void InfoPersonagemFila::entrarNaFila(int tempoChegada) {
    this->presenteNaFila = true;
    this->tempoChegada = tempoChegada;
}

void InfoPersonagemFila::sairDaFila() {
    this->presenteNaFila = false;
    this->nivelPrioridade = Constantes::FORA_FILA;
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

int InfoPersonagemFila::getCodigo() {
    return this->codigoPersonagem;
}

bool InfoPersonagemFila::temMaisPrioridade(InfoPersonagemFila comparacao) {
    if(comparacao.getPrioridade() > this->getPrioridade()) {
        return false;
    }
    
    if(this->getPrioridade() > comparacao.getPrioridade()) {
        return true;
    }

    if(this->getCodigo() >= 6 && comparacao.getCodigo() <= 5) {
        return false;
    }

    if(this->getCodigo() <= 5 && comparacao.getCodigo() >= 6) {
        return true;
    }

    if(this->getCodigo() >= 6 && comparacao.getCodigo() >= 6) {
        return this->getCodigo() < comparacao.getCodigo();
    }

    if(this->getCodigo() <= 5 && comparacao.getCodigo() <= 5) {
        int idtCasal = this->getCodigo()/2;
        int idtCasalComparacao = comparacao.getCodigo()/2;
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