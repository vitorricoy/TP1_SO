#ifndef TRABALHO_PRATICO_SO_SRC_HEADERS_FORNO_H
#define TRABALHO_PRATICO_SO_SRC_HEADERS_FORNO_H

#include <vector>
#include <iostream>
#include <pthread.h>
#include "personagem.h"
#include "info_personagem_fila.h"

using namespace std;

class Forno {

    public: 

        Forno(); // Construtor
        ~Forno(); // Destrutor
        void entrarNaFila(Personagem p); // Indica que um personagem entrou na fila
        void esperar(Personagem p); // Indica que um personagem está esperando para usar o forno
        void liberar(Personagem p); // Libera o personagem que estava usando o forno
        void verificar(); // Verifica que se existe deadlock, e caso exista, os resolve
    
    private:
        // Determina se um personagem está bloqueado por outro, e caso não esteja, sinaliza que ele pode usar o forno
        void determinarBloqueios();
        bool filaVazia(); // Determina se existe algum personagem na fila
        bool podeUsar(int codigoPersonagem); // Determina se o personagem recebido por argumento pode usar o forno
        void atualizarPrioridades(); // Atualiza a prioridade de cada personagem, de acordo com o estado atual da fila
        vector<pthread_cond_t> permissoes; // Vetor que contém as variáveis de condição de cada personagem
        pthread_mutex_t travaForno; // Mutex que controla o acesso ao monitor
        vector<InfoPersonagemFila> personagemFila; // Vetor que guarda as informações de prioridades dos personagens
        int contadorEspera; // Contador de quantas pessoas já entraram na fila. Usada para criar uma ordem de chegada dos personagens
        bool emUso; // Variável que determina se o forno está em uso por algum personagem
};

#endif //TRABALHO_PRATICO_SO_SRC_HEADERS_FORNO_H