#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include "personagem.h"
#include "forno.h"
#include "constantes.h"
#include "info_personagem_fila.h"

using namespace std;

Forno::Forno() {
    // Inicialização das variáveis
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
    // Executa as funções que destõem as variáveis da biblioteca pthread
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        pthread_cond_destroy(&this->permissoes[I]);
    }
    pthread_mutex_destroy(&this->travaForno);
}

void Forno::entrarNaFila(Personagem p) {
    pthread_mutex_lock(&this->travaForno); // Obtém a trava do monitor
    cout << p.getNome() << " quer usar o forno" << endl; // Imprime a mensagem correspondente
    // Indica que o personagem entrou na fila
    this->personagemFila[p.getCodigo()].entrarNaFila(this->contadorEspera++);
    this->atualizarPrioridades(); // Atualiza as prioridades após a alteração da fila
    this->determinarBloqueios(); // Verifica se algum personagem pode usar o forno após essa alteração
    pthread_mutex_unlock(&this->travaForno); // Libera a trava do monitor
}

void Forno::esperar(Personagem p) {
    pthread_mutex_lock(&this->travaForno); // Obtém a trava do monito
    this->atualizarPrioridades(); // Atualiza as prioridades após a alteração da fila
    this->determinarBloqueios(); // Verifica se algum personagem pode usar o forno após essa alteração
    bool jaEstaLiberado = podeUsar(p.getCodigo()); // Verifica se o personagem já pode usar o forno
    if(emUso || !jaEstaLiberado) { // Caso o personagem não possa usar o forno no momento, ele espera a sua vez
        pthread_cond_wait(&this->permissoes[p.getCodigo()], &this->travaForno);
    }
    // Define que o personagem está usando o forno
    emUso = true;
    this->personagemFila[p.getCodigo()].usarForno();

    this->atualizarPrioridades(); // Atualiza as prioridades após a alteração da fila
    pthread_mutex_unlock(&this->travaForno); // Libera a trava do monito
}

void Forno::liberar(Personagem p) {
    pthread_mutex_lock(&this->travaForno); // Obtém a trava do monito
    // Define que o personagem não está mais usando o forno
    emUso = false;
    this->personagemFila[p.getCodigo()].liberarForno();
    this->personagemFila[p.getCodigo()].sairDaFila();

    this->atualizarPrioridades(); // Atualiza as prioridades após a alteração da fila
    this->determinarBloqueios(); // Verifica se algum personagem pode usar o forno após essa alteração
    pthread_mutex_unlock(&this->travaForno); // Libera a trava do monito
}

bool Forno::filaVazia() {
    for(InfoPersonagemFila elemento: this->personagemFila) {
        if(elemento.estaNaFila()) { // A fila contém um personagem, logo não está vazia
            return false;
        }
    }
    return true; // A fila está vazia
}

string Forno::verificar() {
    pthread_mutex_lock(&this->travaForno); // Obtém a trava do monito
    // Variável que contém o nome do personagem liberado do deadlock, ou vazio, caso não exista deadlock
    string retorno = "";

    // Se existem personagens na fila e o forno não está sendo usado
    if(!this->filaVazia() && !this->emUso) {
        // Verifica se alguém pode usar o forno no momento (não existe deadlock)
        bool alguemPodeUsar = false;
        for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
            alguemPodeUsar = alguemPodeUsar || podeUsar(I);
        }
        // Se existem personagens na fila e ninguém pode usar, existe um deadlock
        if(!alguemPodeUsar) {
            // Escolhe um personagem dos três mais prioritários aleatoriamente
            int escolhido = drand48()*3;
            switch(escolhido) {
                case 0:
                    // Define se o casal está na fila, ou apenas um indivíduo, para liberá-los corretamente
                    if(this->personagemFila[Constantes::SHELDON].estaNaFila() && this->personagemFila[Constantes::AMY].estaNaFila()) {
                        this->personagemFila[Constantes::SHELDON].setPrioridade(Constantes::DEADLOCK_FILA);
                        this->personagemFila[Constantes::AMY].setPrioridade(Constantes::DEADLOCK_FILA);
                        // Determina, caso se tenha um casal, qual membro é mais prioritário
                        if(this->personagemFila[Constantes::SHELDON].getTempoChegada() < this->personagemFila[Constantes::AMY].getTempoChegada()) {
                            retorno = Constantes::NOME_SHELDON;
                        } else {
                            retorno = Constantes::NOME_AMY;
                        }
                    } else {
                        if(this->personagemFila[Constantes::SHELDON].estaNaFila()) { // Apenas o Sheldon está na fila
                            this->personagemFila[Constantes::SHELDON].setPrioridade(Constantes::DEADLOCK_FILA);
                            retorno = Constantes::NOME_SHELDON;
                        } else { // Apenas a Amy está na fila
                            this->personagemFila[Constantes::AMY].setPrioridade(Constantes::DEADLOCK_FILA);
                            retorno = Constantes::NOME_AMY;
                        }
                    }
                    break;
                case 1:
                    // Define se o casal está na fila, ou apenas um indivíduo, para liberá-los corretamente
                    if(this->personagemFila[Constantes::HOWARD].estaNaFila() && this->personagemFila[Constantes::BERNADETTE].estaNaFila()) {
                        this->personagemFila[Constantes::HOWARD].setPrioridade(Constantes::DEADLOCK_FILA);
                        this->personagemFila[Constantes::BERNADETTE].setPrioridade(Constantes::DEADLOCK_FILA);
                        // Determina, caso se tenha um casal, qual membro é mais prioritário
                        if(this->personagemFila[Constantes::HOWARD].getTempoChegada() < this->personagemFila[Constantes::BERNADETTE].getTempoChegada()) {
                            retorno = Constantes::NOME_HOWARD;
                        } else {
                            retorno = Constantes::NOME_BERNADETTE;
                        }
                    } else {
                        if(this->personagemFila[Constantes::HOWARD].estaNaFila()) { // Apenas o Howard está na fila
                            this->personagemFila[Constantes::HOWARD].setPrioridade(Constantes::DEADLOCK_FILA);
                            retorno = Constantes::NOME_HOWARD;
                        } else { // Apenas a Bernadette está na fila
                            this->personagemFila[Constantes::BERNADETTE].setPrioridade(Constantes::DEADLOCK_FILA);
                            retorno = Constantes::NOME_BERNADETTE;
                        }
                    }
                    break;
                case 2:
                    // Define se o casal está na fila, ou apenas um indivíduo, para liberá-los corretamente
                    if(this->personagemFila[Constantes::LEONARD].estaNaFila() && this->personagemFila[Constantes::PENNY].estaNaFila()) {
                        this->personagemFila[Constantes::LEONARD].setPrioridade(Constantes::DEADLOCK_FILA);
                        this->personagemFila[Constantes::PENNY].setPrioridade(Constantes::DEADLOCK_FILA);
                        // Determina, caso se tenha um casal, qual membro é mais prioritário
                        if(this->personagemFila[Constantes::LEONARD].getTempoChegada() < this->personagemFila[Constantes::PENNY].getTempoChegada()) {
                            retorno = Constantes::NOME_LEONARD;
                        } else {
                            retorno = Constantes::NOME_PENNY;
                        }
                    } else {
                        if(this->personagemFila[Constantes::LEONARD].estaNaFila()) { // Apenas o Leonard está na fila
                            this->personagemFila[Constantes::LEONARD].setPrioridade(Constantes::DEADLOCK_FILA);
                            retorno = Constantes::NOME_LEONARD;
                        } else { // Apenas a Penny está na fila
                            this->personagemFila[Constantes::PENNY].setPrioridade(Constantes::DEADLOCK_FILA);
                            retorno = Constantes::NOME_PENNY;
                        }
                    }
                    break;
            }
            this->determinarBloqueios(); // Libera os personagens selecionados pelo tratamento do deadlock
        }
    }
    pthread_mutex_unlock(&this->travaForno); // Libera a trava do monito
    return retorno; // Retorna o personagem liberado, ou vazio, caso não tenha ocorido deadlock
}

bool Forno::podeUsar(int codigoPersonagem) {
    // Verifica se o personagem tem mais prioridade que todos os outros
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
    // Determina se um novo casal foi formado após a última alteração
    bool novoCasalFormado = false;
    for(int I=0; I<Constantes::NUMERO_CASAIS; I++) {
        if(this->personagemFila[2*I].estaNaFila() && this->personagemFila[2*I+1].estaNaFila()) { // Se os dois membros do casal estão na fila
            // Se a prioridade dos membros não é de casal
            if(this->personagemFila[2*I].getPrioridade() < Constantes::CASAL_FILA && this->personagemFila[2*I].getPrioridade() < Constantes::CASAL_FILA) {
                // Um novo casal foi formado após a última mudança
                novoCasalFormado = true;
            }
        }
    }

    // Calcula as mudanças de prioridade de cada personagem
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        if(!this->personagemFila[I].estaNaFila()) { // Personagem fora da fila
            this->personagemFila[I].setPrioridade(Constantes::FORA_FILA);
        } else { // Personagem presente na fila
            if(this->personagemFila[I].getPrioridade() == Constantes::DEADLOCK_FILA) { // Personagem foi liberado para resolver deadlock
                continue;
            }
            if(I <= 5) { // O personagem é membro de algum casal
                int idtCasal = I/2; // Identificador do casal do qual o personagem é membro
                // Se ambos os membros do casal estão na fila
                if(this->personagemFila[2*idtCasal].estaNaFila() && this->personagemFila[2*idtCasal+1].estaNaFila()) { 
                    //Se um membro do casal está usando o forno e foi formado um novo casal diferente do que está usando o forno
                    if((this->personagemFila[2*idtCasal].estaUsandoForno() || this->personagemFila[2*idtCasal].estaUsandoForno()) && novoCasalFormado && 
                       this->personagemFila[2*I].getPrioridade() == Constantes::CASAL_FILA && this->personagemFila[2*I].getPrioridade() == Constantes::CASAL_FILA) {
                        
                        // Define que o casal foi desfeito (o novo casal formado terá prioridade para usar o forno)   
                        this->personagemFila[2*idtCasal].setPrioridade(Constantes::SOZINHO_FILA);
                        this->personagemFila[2*idtCasal+1].setPrioridade(Constantes::SOZINHO_FILA);
                    } else { // O casal ganha ou continua com a prioridade de casal
                        this->personagemFila[2*idtCasal].setPrioridade(Constantes::CASAL_FILA);
                        this->personagemFila[2*idtCasal+1].setPrioridade(Constantes::CASAL_FILA);
                    }
                } else { // Personagem está sozinho na fila
                    this->personagemFila[2*idtCasal].setPrioridade(Constantes::SOZINHO_FILA);
                }
            } else { // Kripke e Stuart possuem apenas as prioridades de fora da fila ou sozinho na fila
                this->personagemFila[I].setPrioridade(Constantes::SOZINHO_FILA);
            }
        }
    }
}

void Forno::determinarBloqueios() {
    // Se o forno está em uso ninguém pode ser liberado
    if(emUso) {
        return;
    }
    // Verifica se algum personagem pode usar o forno
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        if(podeUsar(I)) {
            // Caso o personagem possa usar o forno, sua variável de condição é sinalizada, para liberá-lo da espera
            pthread_cond_signal(&permissoes[I]);
        }
    }
}