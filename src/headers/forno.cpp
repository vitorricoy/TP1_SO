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
    this->casais = vector<bool>(Constantes::NUMERO_CASAIS);
    this->casalSheldonAmy = false;
    this->casalHowardBernadette = false;
    this->casalLeonardPenny = false;
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
    if(!emUso || !jaEstaLiberado) {
        pthread_cond_wait(&this->permissoes[p.getCodigo()], &this->travaForno);
    }
    emUso = true;
    pthread_mutex_unlock(&this->travaForno);
}

void Forno::liberar(Personagem p) {
    pthread_mutex_lock(&this->travaForno);
    this->personagemFila[p.getCodigo()].sairDaFila();
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
        if(!this->sheldonPodeUsar() && !this->amyPodeUsar() && !this->howardPodeUsar() && !this->bernadettePodeUsar() 
           && !this->leonardPodeUsar() && !this->pennyPodeUsar() && !this->stuartPodeUsar() && !this->kripkePodeUsar()) {
            int escolhido = drand48()*3;
            switch(escolhido) {
                case 0: 
                    if(casalSheldonAmy && esperando[Constantes::SHELDON] && esperando[Constantes::AMY]) { // Tratando o caso de apenas um dos membros do casal estar na fila com prioridade de casal
                        if(esperando[Constantes::SHELDON] < esperando[Constantes::AMY]) {
                            pthread_cond_signal(&permissoes[Constantes::SHELDON]);
                            retorno = Constantes::NOME_SHELDON;
                        } else {
                            pthread_cond_signal(&permissoes[Constantes::AMY]);
                            retorno = Constantes::NOME_AMY;
                        }
                    } else if(esperando[Constantes::SHELDON]) {
                            pthread_cond_signal(&permissoes[Constantes::SHELDON]);
                            retorno = Constantes::NOME_SHELDON;
                    } else { // AMY esperando
                            pthread_cond_signal(&permissoes[Constantes::AMY]);
                            retorno = Constantes::NOME_AMY;
                    }
                    break;
                case 1:
                    if(casalHowardBernadette && esperando[Constantes::HOWARD] && esperando[Constantes::BERNADETTE]) { // Tratando o caso de apenas um dos membros do casal estar na fila com prioridade de casal
                        if(esperando[Constantes::HOWARD] < esperando[Constantes::BERNADETTE]) {
                            pthread_cond_signal(&permissoes[Constantes::HOWARD]);
                            retorno = Constantes::NOME_HOWARD;
                        } else {
                            pthread_cond_signal(&permissoes[Constantes::BERNADETTE]);
                            retorno = Constantes::NOME_BERNADETTE;
                        }
                    } else if(esperando[Constantes::HOWARD]) {
                            pthread_cond_signal(&permissoes[Constantes::HOWARD]);
                            retorno = Constantes::NOME_HOWARD;
                    } else { // BERNADETTE esperando
                            pthread_cond_signal(&permissoes[Constantes::BERNADETTE]);
                            retorno = Constantes::NOME_BERNADETTE;
                    }
                    break;
                case 2:
                    if(casalLeonardPenny && esperando[Constantes::LEONARD] && esperando[Constantes::PENNY]) { // Tratando o caso de apenas um dos membros do casal estar na fila com prioridade de casal
                        if(esperando[Constantes::LEONARD] < esperando[Constantes::PENNY]) {
                            pthread_cond_signal(&permissoes[Constantes::LEONARD]);
                            retorno = Constantes::NOME_LEONARD;
                        } else {
                            pthread_cond_signal(&permissoes[Constantes::PENNY]);
                            retorno = Constantes::NOME_PENNY;
                        }
                    } else if(esperando[Constantes::LEONARD]) {
                            pthread_cond_signal(&permissoes[Constantes::LEONARD]);
                            retorno = Constantes::NOME_LEONARD;
                    } else { // PENNY esperando
                            pthread_cond_signal(&permissoes[Constantes::PENNY]);
                            retorno = Constantes::NOME_PENNY;
                    }
                    break;
            }
        }
    }
    pthread_mutex_unlock(&this->travaForno);
    return retorno;
}

//Considera que o privilegio de casal acaba somente quanto ambos usam o forno e saem da fila
void Forno::atualizarCasais() {
    
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
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        if(!this->personagemFila[I].estaNaFila()) {
            this->personagemFila[I].setPrioridade(Constantes::FORA_FILA);
        } else {
            if(I >= 5) {
                int idtCasal = I/2;
                

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

    atualizarCasais();

    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        if(podeUsar(I)) {
            pthread_cond_signal(&permissoes[I]);
        }
    }
}