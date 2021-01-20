#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "headers/forno.h"
#include "headers/personagem.h"
#include "headers/constantes.h"

using namespace std;

void esquentar_algo(Personagem p) {
    cout << p.getNome() << " começa a esquentar algo" << endl;
    sleep(1); // Tempo que demora para esquentar a comida no forno
}

void comer(Personagem p) {
    cout << p.getNome() << " vai comer" << endl;
    sleep(3); // Tempo que o personagem demora para comer
}

void trabalhar(Personagem p) {
    cout << p.getNome() << " voltou para o trabalho" << endl;
    sleep(10+drand48()*6); // Tempo que o personagem fica no trabalho (De 10 a 15 segundos)
}

int vezes; // Número de vezes que os personagens devem usar o forno

Forno forno = Forno(); // Monitor que controla o acesso ao forno

bool ativo; // Flag para indicar que existe algum personagem que ainda pode usar o forno

void* rotina(void* personagem) {
    // Dados do personagem
    Personagem p = *((Personagem*) personagem);
    // O personagem executa sua rotina um determinado número de vezes
    for(int i=0; i < vezes; i++) {
        forno.entrarNaFila(p); // Registra que o personagem quer usar o forno, mas ainda não pode usá-lo de fato
        usleep(300000); // Sleep para que o personagem não use o forno instantaneamente, caso seja o mais prioritário, permitindo que se tenha deadlocks
        forno.esperar(p); // Personagem espera o forno estar liberado para usá-lo
        esquentar_algo(p);
        forno.liberar(p);
        comer(p);
        trabalhar(p);
    }
    // Encerra a thread do personagem
    pthread_exit(0);
}

void* raj(void* arg) {
    while(ativo) { // Executa enquanto existir um personagem que pode usar o forno
        sleep(5); // Espera um tempo para verificar o deadlock
        string liberado = forno.verificar(); // Verifica se existe deadlock, e caso exista, o resolve
        if(!liberado.empty()) { // Se a rotina de verificação de deadlocks liberou alguém, imprime a mensagem
            cout << "Raj detectou um deadlock, liberando " << liberado << endl;
        }
    }
    // Encerra a thread do Raj
    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    // Recebe o número de vezes que os personagens usarão o forno por argumento
    string argumento = argv[1];
    vezes = stoi(argumento);

    // Declara o vetor de threads dos personagems
    pthread_t thread_handles[Constantes::NUMERO_PERSONAGENS];

    // Declara o vetor de objetos que contém as informações dos personagens
    Personagem personagens[Constantes::NUMERO_PERSONAGENS];
    personagens[0] = Personagem(Constantes::NOME_SHELDON, Constantes::SHELDON);
    personagens[1] = Personagem(Constantes::NOME_AMY, Constantes::AMY);
    personagens[2] = Personagem(Constantes::NOME_HOWARD, Constantes::HOWARD);
    personagens[3] = Personagem(Constantes::NOME_BERNADETTE, Constantes::BERNADETTE);
    personagens[4] = Personagem(Constantes::NOME_LEONARD, Constantes::LEONARD);
    personagens[5] = Personagem(Constantes::NOME_PENNY, Constantes::PENNY);
    personagens[6] = Personagem(Constantes::NOME_STUART, Constantes::STUART);
    personagens[7] = Personagem(Constantes::NOME_KRIPKE, Constantes::KRIPKE);
    
    // Determina que existem threads ativas
    ativo = true;

    //Cria a thread do Raj
    pthread_t thread_raj;
    errno = pthread_create(&thread_raj, NULL, raj, NULL);
    if(errno) { // Verifica se houve erro
        perror(NULL);
    }

    //Cria as threads dos personagens
    for(int thread = 0; thread < Constantes::NUMERO_PERSONAGENS; thread++) {
        errno = pthread_create(&thread_handles[thread], NULL, rotina, &personagens[thread]);
        if(errno) { // Verifica se houve erro
            perror(NULL);
        }
    }

    // Espera as threads dos personagens acabarem de executar
    for(int thread = 0; thread < Constantes::NUMERO_PERSONAGENS; thread++) {
        errno = pthread_join(thread_handles[thread], NULL);
        if(errno) { // Verifica se houve erro
            perror(NULL);
        }
    }

    // Define que não existem mais personagens que podem usar o forno
    ativo = false;

    // Espera a thread do Raj acabar sua execução
    errno = pthread_join(thread_raj, NULL);
    if(errno) { // Verifica se houve erro
        perror(NULL);
    }

    return 0;
}