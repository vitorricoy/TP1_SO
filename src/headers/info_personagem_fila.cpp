#include <iostream>
#include "info_personagem_fila.h"
#include "constantes.h"

using namespace std;

InfoPersonagemFila::InfoPersonagemFila() {
    // Inicializa as variáveis com os valores nulos
    this->nivelPrioridade = 0;
    this->tempoChegada = 0;
    this->presenteNaFila = false;
    this->codigoPersonagem = -1;
    this->usandoForno = false;
    this->casalDesfeito = false;
}

void InfoPersonagemFila::setPrioridade(int prioridade) {
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

bool InfoPersonagemFila::casalFoiDesfeito() {
    return this->casalDesfeito;
}

bool InfoPersonagemFila::estaNaFila() {
    return this->presenteNaFila;
}

bool InfoPersonagemFila::estaUsandoForno() {
    return this->usandoForno;
}

void InfoPersonagemFila::usarForno() {
    this->usandoForno = true;
}

void InfoPersonagemFila::liberarForno() {
    this->usandoForno = false;
}

void InfoPersonagemFila::setCasalDesfeito(bool casalDesfeito) {
    this->casalDesfeito = casalDesfeito;
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
    // Se a prioridade superior do outro personagem for maior, ou se eu não estiver na fila, tenho menos prioridade
    if(!this->estaNaFila() || comparacao.getPrioridade() > this->getPrioridade()) {
        return false;
    }
    // Se a minha prioridade superior for maior, ou o outro personagem não estiver na fila, tenho mais prioridade
    if(!comparacao.estaNaFila() || this->getPrioridade() > comparacao.getPrioridade()) {
        return true;
    }
    // Se eu sou o Kripke ou Stuart e o outro personagem é um membro de casal, tenho menos prioridade
    if(this->getCodigo() >= 6 && comparacao.getCodigo() <= 5) {
        return false;
    }
    // Se eu sou um membro de casal e o outro personagem é o Kripke ou Stuart, eu tenho mais prioridade
    if(this->getCodigo() <= 5 && comparacao.getCodigo() >= 6) {
        return true;
    }
    // Se eu e o outro personagem não somos membros de casais, quem for o Stuart tem mais prioridade
    if(this->getCodigo() >= 6 && comparacao.getCodigo() >= 6) {
        return this->getCodigo() < comparacao.getCodigo();
    }
    // Se eu e o outro personagem somos membros de casais
    if(this->getCodigo() <= 5 && comparacao.getCodigo() <= 5) {
        int idtCasal = this->getCodigo()/2; // Identificador do meu casal
        int idtCasalComparacao = comparacao.getCodigo()/2; // Indentificador do casal do outro personagem
        if(idtCasal == idtCasalComparacao) { 
            // Se somos membros do mesmo casal
            // Quem chegou primeiro tem mais prioridade
            return this->getTempoChegada() < comparacao.getTempoChegada();
        } else {
            // Verifica qual membro de casal tem maior prioridade
            // Como as prioridades superiores já foram verificadas, ambos personagens estão sozinhos ou em casais
            if((idtCasal+1)%Constantes::NUMERO_CASAIS == idtCasalComparacao) {
                return true;
            } else {
                return false;
            }
        }
    }
    // Garantia de que, caso ocorra algum bug, o personagem não é liberado
    return false;
}