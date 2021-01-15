#include <iostream>
#include <vector>
#include <pthread.h>
#include "personagem.h"
#include "forno.h"
#include "constantes.h"
#include "info_personagem_fila.h"

using namespace std;

struct InformacoesFila {
    
};

Forno::Forno(){
    this->permissoes = vector<pthread_cond_t>(Constantes::NUMERO_PERSONAGENS);
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        pthread_cond_init(&this->permissoes[I], NULL);
    }
    pthread_mutex_init(&this->travaForno, NULL);
    this->personagemFila = vector<InfoPersonagemFila>(Constantes::NUMERO_PERSONAGENS);
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        this->personagemFila[I].setCodigo(I);
    }
    this->novoCasalFormado = false;
    this->contadorEspera = 1;
    this->emUso = false;
}

Forno::~Forno() {
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        pthread_cond_destroy(&this->permissoes[I]);
    }
    pthread_mutex_destroy(&this->travaForno);
}

void Forno::esperar(Personagem p) {
    pthread_mutex_lock(&this->travaForno);
    cout << p.getNome() << " quer usar o forno" << endl;
    this->personagemFila[p.getCodigo()].entrarNaFila(this->contadorEspera++);
    this->atualizarPrioridades();
    this->determinarBloqueios();
    bool jaEstaLiberado = podeUsar(p.getCodigo());
    if(emUso || !jaEstaLiberado) {
        pthread_cond_wait(&this->permissoes[p.getCodigo()], &this->travaForno);
    }
    emUso = true;
    this->personagemFila[p.getCodigo()].sairDaFila();
    this->atualizarPrioridades();
    pthread_mutex_unlock(&this->travaForno);
}

void Forno::liberar(Personagem p) {
    pthread_mutex_lock(&this->travaForno);
    emUso = false;
    this->atualizarPrioridades();
    this->determinarBloqueios();
    pthread_mutex_unlock(&this->travaForno);
}

bool Forno::filaVazia() {
    for(InfoPersonagemFila elemento: this->personagemFila) {
        if(elemento.estaNaFila()) {
            return false;
        }
    }
    return true;
}

string Forno::verificar() {
    pthread_mutex_lock(&this->travaForno);
    string retorno = "";

    if(!this->filaVazia() && !this->emUso) {
        bool alguemPodeUsar = false;
        for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
            alguemPodeUsar = alguemPodeUsar || podeUsar(I);
        }
        if(!alguemPodeUsar) {
            int escolhido = drand48()*3;
            switch(escolhido) {
                case 0:
                    if(this->personagemFila[Constantes::SHELDON].estaNaFila() && this->personagemFila[Constantes::AMY].estaNaFila()) {
                        this->personagemFila[Constantes::SHELDON].setPrioridade(Constantes::CASAL_DEADLOCK_FILA);
                        this->personagemFila[Constantes::AMY].setPrioridade(Constantes::CASAL_DEADLOCK_FILA);
                    } else {
                        if(this->personagemFila[Constantes::SHELDON].estaNaFila()) {
                            this->personagemFila[Constantes::SHELDON].setPrioridade(Constantes::CASAL_DEADLOCK_FILA);
                        } else {
                            this->personagemFila[Constantes::AMY].setPrioridade(Constantes::CASAL_DEADLOCK_FILA);
                        }
                    }
                    break;
                case 1:
                    if(this->personagemFila[Constantes::HOWARD].estaNaFila() && this->personagemFila[Constantes::BERNADETTE].estaNaFila()) {
                        this->personagemFila[Constantes::HOWARD].setPrioridade(Constantes::CASAL_DEADLOCK_FILA);
                        this->personagemFila[Constantes::BERNADETTE].setPrioridade(Constantes::CASAL_DEADLOCK_FILA);
                    } else {
                        if(this->personagemFila[Constantes::HOWARD].estaNaFila()) {
                            this->personagemFila[Constantes::HOWARD].setPrioridade(Constantes::CASAL_DEADLOCK_FILA);
                        } else {
                            this->personagemFila[Constantes::BERNADETTE].setPrioridade(Constantes::CASAL_DEADLOCK_FILA);
                        }
                    }
                    break;
                case 2:
                    if(this->personagemFila[Constantes::LEONARD].estaNaFila() && this->personagemFila[Constantes::PENNY].estaNaFila()) {
                        this->personagemFila[Constantes::LEONARD].setPrioridade(Constantes::CASAL_DEADLOCK_FILA);
                        this->personagemFila[Constantes::PENNY].setPrioridade(Constantes::CASAL_DEADLOCK_FILA);
                    } else {
                        if(this->personagemFila[Constantes::LEONARD].estaNaFila()) {
                            this->personagemFila[Constantes::LEONARD].setPrioridade(Constantes::CASAL_DEADLOCK_FILA);
                        } else {
                            this->personagemFila[Constantes::PENNY].setPrioridade(Constantes::CASAL_DEADLOCK_FILA);
                        }
                    }
                    break;
            }
            this->determinarBloqueios();
        }
    }
    pthread_mutex_unlock(&this->travaForno);
    return retorno;
}

bool Forno::podeUsar(int codigoPersonagem) {
    // Verifica se o personagem tem mais prioridade que todos
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        if(I != codigoPersonagem) {
            if(!this->personagemFila[codigoPersonagem].temMaisPrioridade(this->personagemFila[I])) {
                return false;
            }
        }
    }
    return true;
}

void Forno::atualizarPrioridades() {
    for(int I=0; I<Constantes::NUMERO_CASAIS; I++) {
        if(this->personagemFila[2*I].estaNaFila() && this->personagemFila[2*I+1].estaNaFila()) {
            if(this->personagemFila[2*I].getPrioridade() < Constantes::CASAL_FILA && this->personagemFila[2*I].getPrioridade() < Constantes::CASAL_FILA) {
                this->novoCasalFormado = true;
            }
        }
    }
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        if(!this->personagemFila[I].estaNaFila()) {
            this->personagemFila[I].setPrioridade(Constantes::FORA_FILA);
        } else {
            if(I <= 5) {
                int idtCasal = I/2;
                if(this->personagemFila[2*idtCasal].estaNaFila() && this->personagemFila[2*idtCasal+1].estaNaFila()) {
                    this->personagemFila[2*idtCasal].setPrioridade(Constantes::CASAL_FILA);
                    this->personagemFila[2*idtCasal+1].setPrioridade(Constantes::CASAL_FILA);
                } else {
                    if(this->personagemFila[I].getPrioridade() == Constantes::CASAL_FILA) { // Namorado(a) usando forno
                        if(this->novoCasalFormado) {
                            this->personagemFila[I].setPrioridade(Constantes::CASAL_DESFEITO_FILA);
                        }
                    } else {
                        this->personagemFila[I].setPrioridade(Constantes::SOZINHO_FILA);
                    }
                }
            } else {
                this->personagemFila[I].setPrioridade(Constantes::SOZINHO_FILA);
            }
        }
    }
    this->novoCasalFormado = false;
}

void Forno::determinarBloqueios() {

    if(emUso) {
        return;
    }

    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        if(podeUsar(I)) {
            pthread_cond_signal(&permissoes[I]);
        }
    }
}