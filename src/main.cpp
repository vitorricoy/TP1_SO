#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "headers/forno.h"
#include "headers/personagem.h"
#include "headers/constantes.h"

using namespace std;

void esquentar_algo(Personagem p) {
    cout << p.getNome() << " começa a esquentar algo" << endl;
    sleep(1);
}

void comer(Personagem p) {
    cout << p.getNome() << " vai comer" << endl;
    sleep(3);
}

void trabalhar(Personagem p) {
    cout << p.getNome() << " voltou para o trabalho" << endl;
    sleep(10+drand48()*6);
}

int vezes;

Forno forno = Forno();

bool ativo;

void* rotina(void* personagem) {
    Personagem p = *((Personagem*) personagem);
    for(int i=0; i < vezes; i++) {
        forno.entrarNaFila(p); // Registra que o personagem quer usar o forno, mas ainda não pode usá-lo de fato
        usleep(300000); // Sleep para que o personagem não use o forno instantaneamente, caso seja o mais prioritário, permitindo que se tenha deadlocks
        forno.esperar(p); // Personagem espera o forno estar liberado para usá-lo
        esquentar_algo(p);
        forno.liberar(p);
        comer(p);
        trabalhar(p);
    }
    pthread_exit(0);
}

void* raj(void* arg) {
    while(ativo) {
        sleep(5);
        string liberado = forno.verificar();
        if(!liberado.empty()) {
            cout << "Raj detectou um deadlock, liberando " << liberado << endl;
        }
    }

    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    string argumento = argv[1];
    vezes = stoi(argumento);

    long int thread;
    pthread_t thread_handles[Constantes::NUMERO_PERSONAGENS];

    Personagem personagens[Constantes::NUMERO_PERSONAGENS];
    personagens[0] = Personagem(Constantes::NOME_SHELDON, Constantes::SHELDON);
    personagens[1] = Personagem(Constantes::NOME_AMY, Constantes::AMY);
    personagens[2] = Personagem(Constantes::NOME_HOWARD, Constantes::HOWARD);
    personagens[3] = Personagem(Constantes::NOME_BERNADETTE, Constantes::BERNADETTE);
    personagens[4] = Personagem(Constantes::NOME_LEONARD, Constantes::LEONARD);
    personagens[5] = Personagem(Constantes::NOME_PENNY, Constantes::PENNY);
    personagens[6] = Personagem(Constantes::NOME_STUART, Constantes::STUART);
    personagens[7] = Personagem(Constantes::NOME_KRIPKE, Constantes::KRIPKE);

    ativo = true;

    pthread_t thread_raj;
    errno = pthread_create(&thread_raj, NULL, raj, NULL);
    if(errno) {
        perror(NULL);
    }

    for (thread = 0; thread < Constantes::NUMERO_PERSONAGENS; thread++) {
        errno = pthread_create(&thread_handles[thread], NULL, rotina, &personagens[thread]);
        if(errno) {
            perror(NULL);
        }
    }

    for (thread = 0; thread < Constantes::NUMERO_PERSONAGENS; thread++) {
        errno = pthread_join(thread_handles[thread], NULL);
        if(errno) {
            perror(NULL);
        }
    }

    ativo = false;

    errno = pthread_join(thread_raj, NULL);
    if(errno) {
        perror(NULL);
    }

    return 0;
}