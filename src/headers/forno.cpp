#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h>
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
    this->contadorEspera = 1;
    this->emUso = false;
}

Forno::~Forno() {
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        pthread_cond_destroy(&this->permissoes[I]);
    }
    pthread_mutex_destroy(&this->travaForno);
}

void Forno::entrarNaFila(Personagem p) {
    pthread_mutex_lock(&this->travaForno);
    cout << p.getNome() << " quer usar o forno" << endl;
    this->personagemFila[p.getCodigo()].entrarNaFila(this->contadorEspera++);
    this->atualizarPrioridades();
    this->determinarBloqueios();
    pthread_mutex_unlock(&this->travaForno);
}

void Forno::esperar(Personagem p) {
    pthread_mutex_lock(&this->travaForno);
    this->atualizarPrioridades();
    this->determinarBloqueios();
    bool jaEstaLiberado = podeUsar(p.getCodigo());
    if(emUso || !jaEstaLiberado) {
        pthread_cond_wait(&this->permissoes[p.getCodigo()], &this->travaForno);
    }
    emUso = true;
    this->personagemFila[p.getCodigo()].usarForno();
    this->atualizarPrioridades();
    pthread_mutex_unlock(&this->travaForno);
}

void Forno::liberar(Personagem p) {
    pthread_mutex_lock(&this->travaForno);
    emUso = false;
    this->personagemFila[p.getCodigo()].liberarForno();
    this->personagemFila[p.getCodigo()].sairDaFila();
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
                        this->personagemFila[Constantes::SHELDON].setPrioridade(Constantes::DEADLOCK_FILA);
                        this->personagemFila[Constantes::AMY].setPrioridade(Constantes::DEADLOCK_FILA);
                        if(this->personagemFila[Constantes::SHELDON].getTempoChegada() < this->personagemFila[Constantes::AMY].getTempoChegada()) {
                            retorno = Constantes::NOME_SHELDON;
                        } else {
                            retorno = Constantes::NOME_AMY;
                        }
                    } else {
                        if(this->personagemFila[Constantes::SHELDON].estaNaFila()) {
                            this->personagemFila[Constantes::SHELDON].setPrioridade(Constantes::DEADLOCK_FILA);
                            retorno = Constantes::NOME_SHELDON;
                        } else {
                            this->personagemFila[Constantes::AMY].setPrioridade(Constantes::DEADLOCK_FILA);
                            retorno = Constantes::NOME_AMY;
                        }
                    }
                    break;
                case 1:
                    if(this->personagemFila[Constantes::HOWARD].estaNaFila() && this->personagemFila[Constantes::BERNADETTE].estaNaFila()) {
                        this->personagemFila[Constantes::HOWARD].setPrioridade(Constantes::DEADLOCK_FILA);
                        this->personagemFila[Constantes::BERNADETTE].setPrioridade(Constantes::DEADLOCK_FILA);
                        if(this->personagemFila[Constantes::HOWARD].getTempoChegada() < this->personagemFila[Constantes::BERNADETTE].getTempoChegada()) {
                            retorno = Constantes::NOME_HOWARD;
                        } else {
                            retorno = Constantes::NOME_BERNADETTE;
                        }
                    } else {
                        if(this->personagemFila[Constantes::HOWARD].estaNaFila()) {
                            this->personagemFila[Constantes::HOWARD].setPrioridade(Constantes::DEADLOCK_FILA);
                            retorno = Constantes::NOME_HOWARD;
                        } else {
                            this->personagemFila[Constantes::BERNADETTE].setPrioridade(Constantes::DEADLOCK_FILA);
                            retorno = Constantes::NOME_BERNADETTE;
                        }
                    }
                    break;
                case 2:
                    if(this->personagemFila[Constantes::LEONARD].estaNaFila() && this->personagemFila[Constantes::PENNY].estaNaFila()) {
                        this->personagemFila[Constantes::LEONARD].setPrioridade(Constantes::DEADLOCK_FILA);
                        this->personagemFila[Constantes::PENNY].setPrioridade(Constantes::DEADLOCK_FILA);
                        if(this->personagemFila[Constantes::LEONARD].getTempoChegada() < this->personagemFila[Constantes::PENNY].getTempoChegada()) {
                            retorno = Constantes::NOME_LEONARD;
                        } else {
                            retorno = Constantes::NOME_PENNY;
                        }
                    } else {
                        if(this->personagemFila[Constantes::LEONARD].estaNaFila()) {
                            this->personagemFila[Constantes::LEONARD].setPrioridade(Constantes::DEADLOCK_FILA);
                            retorno = Constantes::NOME_LEONARD;
                        } else {
                            this->personagemFila[Constantes::PENNY].setPrioridade(Constantes::DEADLOCK_FILA);
                            retorno = Constantes::NOME_PENNY;
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
    bool novoCasalFormado = false;
    for(int I=0; I<Constantes::NUMERO_CASAIS; I++) {
        if(this->personagemFila[2*I].estaNaFila() && this->personagemFila[2*I+1].estaNaFila()) {
            if(this->personagemFila[2*I].getPrioridade() < Constantes::CASAL_FILA && this->personagemFila[2*I].getPrioridade() < Constantes::CASAL_FILA) {
                novoCasalFormado = true;
            }
        }
    }
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        if(!this->personagemFila[I].estaNaFila()) {
            this->personagemFila[I].setPrioridade(Constantes::FORA_FILA);
        } else {
            if(this->personagemFila[I].getPrioridade() == Constantes::DEADLOCK_FILA) {
                continue;
            }
            if(I <= 5) {
                int idtCasal = I/2;
                if(this->personagemFila[2*idtCasal].estaNaFila() && this->personagemFila[2*idtCasal+1].estaNaFila()) {
                    if((this->personagemFila[2*idtCasal].estaUsandoForno() || this->personagemFila[2*idtCasal].estaUsandoForno()) && novoCasalFormado && this->personagemFila[2*I].getPrioridade() == Constantes::CASAL_FILA && this->personagemFila[2*I].getPrioridade() == Constantes::CASAL_FILA) {
                        this->personagemFila[2*idtCasal].setPrioridade(Constantes::SOZINHO_FILA);
                        this->personagemFila[2*idtCasal+1].setPrioridade(Constantes::SOZINHO_FILA);
                    } else {
                        this->personagemFila[2*idtCasal].setPrioridade(Constantes::CASAL_FILA);
                        this->personagemFila[2*idtCasal+1].setPrioridade(Constantes::CASAL_FILA);
                    }
                }
            } else {
                this->personagemFila[I].setPrioridade(Constantes::SOZINHO_FILA);
            }
        }
    }
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