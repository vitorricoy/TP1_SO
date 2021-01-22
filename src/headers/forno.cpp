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
    // Executa as funções que destrõem as variáveis da biblioteca pthread
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
    pthread_mutex_lock(&this->travaForno); // Obtém a trava do monitor
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
    pthread_mutex_unlock(&this->travaForno); // Libera a trava do monitor
}

void Forno::liberar(Personagem p) {
    pthread_mutex_lock(&this->travaForno); // Obtém a trava do monitor
    // Define que o personagem não está mais usando o forno
    emUso = false;
    this->personagemFila[p.getCodigo()].liberarForno();
    this->personagemFila[p.getCodigo()].sairDaFila();

    this->atualizarPrioridades(); // Atualiza as prioridades após a alteração da fila
    this->determinarBloqueios(); // Verifica se algum personagem pode usar o forno após essa alteração
    // Imprime a mensagem de que a pessoa foi comer
    // Isso é feito dentro do forno para evitar de alguém começar a esquentar antes da pessoa ir comer
    cout << p.getNome() << " vai comer" << endl; 
    pthread_mutex_unlock(&this->travaForno); // Libera a trava do monitor
}

bool Forno::filaVazia() {
    for(InfoPersonagemFila elemento: this->personagemFila) {
        if(elemento.estaNaFila()) { // A fila contém um personagem, logo não está vazia
            return false;
        }
    }
    return true; // A fila está vazia
}

void Forno::verificar() {
    pthread_mutex_lock(&this->travaForno); // Obtém a trava do monitor
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
            int casalEscolhido = drand48()*3;
            string saida, nomeMembro1, nomeMembro2;

            // Guarda o nome do casal sorteado para a saída de quem foi liberado
            if(casalEscolhido == 0) {
                nomeMembro1 = Constantes::NOME_SHELDON; 
                nomeMembro2 = Constantes::NOME_AMY; 
            } else {
                if(casalEscolhido == 1) {
                    nomeMembro1 = Constantes::NOME_HOWARD; 
                    nomeMembro2 = Constantes::NOME_BERNADETTE;
                } else {
                    nomeMembro1 = Constantes::NOME_LEONARD; 
                    nomeMembro2 = Constantes::NOME_PENNY;
                }
            }
            // Define se o casal está na fila, ou apenas um indivíduo, para liberá-los corretamente
            if(this->personagemFila[2*casalEscolhido].estaNaFila() && this->personagemFila[2*casalEscolhido+1].estaNaFila()) {
                this->personagemFila[2*casalEscolhido].setPrioridade(Constantes::DEADLOCK_FILA);
                this->personagemFila[2*casalEscolhido+1].setPrioridade(Constantes::DEADLOCK_FILA);
                // Determina, caso se tenha um casal, qual membro é mais prioritário
                if(this->personagemFila[2*casalEscolhido].getTempoChegada() < this->personagemFila[2*casalEscolhido+1].getTempoChegada()) {
                    saida = nomeMembro1;
                } else {
                    saida = nomeMembro2;
                }
            } else {
                if(this->personagemFila[2*casalEscolhido].estaNaFila()) { 
                    // Apenas o primeiro membro está na fila
                    this->personagemFila[2*casalEscolhido].setPrioridade(Constantes::DEADLOCK_FILA);
                    saida = nomeMembro1;
                } else { 
                    // Apenas o segundo membro está na fila
                    this->personagemFila[2*casalEscolhido+1].setPrioridade(Constantes::DEADLOCK_FILA);
                    saida = nomeMembro2;
                }
            }
            cout << "Raj detectou um deadlock, liberando " << saida << endl; // Imprime a mensagem de personagem liberado
            this->determinarBloqueios(); // Libera os personagens selecionados pelo tratamento do deadlock
        }
    }
    pthread_mutex_unlock(&this->travaForno); // Libera a trava do monitor
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
            if(this->personagemFila[2*I].getPrioridade() < Constantes::CASAL_FILA || this->personagemFila[2*I+1].getPrioridade() < Constantes::CASAL_FILA) {
                // Um novo casal foi formado após a última mudança
                novoCasalFormado = true;
            }
        }
    }
    // Calcula as mudanças de prioridade de cada personagem
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        if(!this->personagemFila[I].estaNaFila()) { // Personagem fora da fila
            this->personagemFila[I].setPrioridade(Constantes::FORA_FILA);
            if(I <= 5) { // O personagem é membro de casal
                int idtCasal = I/2;
                // Define que caso esse casal tenha sido desfeito, ele pode ser refeito, já que um dos membros já deixou a fila
                this->personagemFila[idtCasal].setCasalDesfeito(false);
                this->personagemFila[idtCasal+1].setCasalDesfeito(false);
            }
        } else { // Personagem presente na fila
            // Personagem foi liberado para resolver deadlock ou está usando o forno, logo não precisa atualizar prioridades
            if(this->personagemFila[I].getPrioridade() == Constantes::DEADLOCK_FILA || this->personagemFila[I].estaUsandoForno()) {
                continue;
            }
            if(I <= 5) { // O personagem é membro de algum casal
                int idtCasal = I/2; // Identificador do casal do qual o personagem é membro
                // Se ambos os membros do casal estão na fila
                if(this->personagemFila[2*idtCasal].estaNaFila() && this->personagemFila[2*idtCasal+1].estaNaFila()) {
                    //Se um membro do casal está usando o forno e foi formado um novo casal diferente do que está usando o forno
                    if((this->personagemFila[2*idtCasal].estaUsandoForno() || this->personagemFila[2*idtCasal+1].estaUsandoForno()) && novoCasalFormado && 
                       this->personagemFila[2*idtCasal].getPrioridade() == Constantes::CASAL_FILA && this->personagemFila[2*idtCasal+1].getPrioridade() == Constantes::CASAL_FILA) {
                        
                        // Define que o casal foi desfeito (o novo casal formado terá prioridade para usar o forno)
                        this->personagemFila[I].setPrioridade(Constantes::SOZINHO_FILA);
                        // Variável de controle que não permite que esse casal seja refeito até que o outro membro termine de usar o forno e reentre na fila
                        this->personagemFila[I].setCasalDesfeito(true); 
                    } else { // O casal não foi desfeito no momento
                        if(!this->personagemFila[I].casalFoiDesfeito()) { // Verifica se o casal já foi desfeito
                            // Se o casal não foi desfeito, pode ganhar a prioridade de casal
                            this->personagemFila[I].setPrioridade(Constantes::CASAL_FILA);
                        }
                    }
                } else { // Personagem está sozinho na fila
                    // Se tinha um casal, o outro membro deve usar em seguida, logo mantém a prioridade de casal para ele
                    if(this->personagemFila[I].getPrioridade() < Constantes::CASAL_FILA) { 
                        this->personagemFila[I].setPrioridade(Constantes::SOZINHO_FILA);
                    }
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