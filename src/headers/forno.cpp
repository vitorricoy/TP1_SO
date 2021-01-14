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

    for(int I=0; I<Constantes::NUMERO_CASAIS; I++) {
        if(this->casais[I]) {
            this->casais[I] = esperando[2*I] || esperando[2*I + 1];
        } else {
            this->casais[I] = esperando[2*I] && esperando[2*I + 1];
        }
    }

    if(casalSheldonAmy) {
        casalSheldonAmy = esperando[Constantes::SHELDON] || esperando[Constantes::AMY];
    } else {
        casalSheldonAmy = esperando[Constantes::SHELDON] && esperando[Constantes::AMY];
    }

    if(casalHowardBernadette) {
        casalHowardBernadette = esperando[Constantes::HOWARD] || esperando[Constantes::BERNADETTE];
    } else {
        casalHowardBernadette = esperando[Constantes::HOWARD] && esperando[Constantes::BERNADETTE];
    }

    if(casalLeonardPenny) {
        casalLeonardPenny = esperando[Constantes::LEONARD] || esperando[Constantes::PENNY];
    } else {
        casalLeonardPenny = esperando[Constantes::LEONARD] && esperando[Constantes::PENNY];
    }
}

bool Forno::podeUsar(int codigoPersonagem) {
    if(this->personagemFila[codigoPersonagem].estaNaFila()) {
        auto modPositivo = [](int x, int mod) { return ((x%mod)+mod)%mod; };
        // Obtém a prioridade do personagem de maior prioridade na fila
        int maiorPrioridade = 0;
        for(InfoPersonagemFila info : this->personagemFila) {
            if(info.getPrioridade() > maiorPrioridade) {
                maiorPrioridade = info.getPrioridade();
            }
        }
        // Se o personagem atual tiver uma prioridade menor que alguém da fila, ele não pode usar o forno
        if(this->personagemFila[codigoPersonagem].getPrioridade() < maiorPrioridade) {
            return false;
        }
        if(codigoPersonagem <= 5) { // Personagem pode ser membro de casal
            int codigoCasalMaiorPrioridade = modPositivo(codigoPersonagem/2-1, Constantes::NUMERO_CASAIS);
            int codigoCasalAtual = codigoPersonagem/2;
            bool existeCasal = false;
            for(bool el : this->casais) {
                existeCasal = existeCasal || el;
            }
            bool casalPresente = this->casais[codigoCasalAtual]; // Obtém se o casal do personagem está presente
            if(existeCasal && casalPresente) {
                if(!this->casais[codigoCasalMaiorPrioridade]) { // Verifica o casal anterior na lista de prioridades (que tem mais prioridade)
                    if(codigoPersonagem%2 == 0) { // É o membro masculino do casal
                        if(this->esperando[codigoPersonagem] < this->esperando[codigoPersonagem+1] || !this->esperando[codigoPersonagem+1]) {
                            return true;
                        }
                    } else { // É o membro feminino
                        if(this->esperando[codigoPersonagem] < this->esperando[codigoPersonagem-1] || !this->esperando[codigoPersonagem-1]) {
                            return true;
                        }
                    }
                }
            } else {
                if(!existeCasal) { // Não tem casal
                    if(!this->esperando[codigoCasalMaiorPrioridade*2] && !this->esperando[codigoCasalMaiorPrioridade*2 + 1]) { // Verifica se nenhum personagem com mais prioridade está esperando na fila
                        return true;
                    }
                }
            }
        } else { // Stuart e Kripke
            bool algumPrioritarioEsperando = false;
            for(int I=0; I<codigoPersonagem; I++) {
                algumPrioritarioEsperando = algumPrioritarioEsperando || this->esperando[I];
            }
            if(!algumPrioritarioEsperando) {
                return true;
            }
        }
    }
    return false;
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