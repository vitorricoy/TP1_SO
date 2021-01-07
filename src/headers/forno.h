#ifndef TRABALHO_PRATICO_SO_SRC_HEADERS_FORNO_H
#define TRABALHO_PRATICO_SO_SRC_HEADERS_FORNO_H

#include <map>
#include <iostream>
#include <pthread.h>
#include "personagem.h"
#include "raj.h"

using namespace std;

class Forno {

    public: 

        Forno(map<string, pthread_cond_t>& permissoes, pthread_mutex_t& forno);

        void esperar(Personagem p);

        void liberar(Personagem p);

        void verificar();

        bool pegarForno(Personagem p);
    
    private:
        
        void determinarBloqueios();
        bool verificarPermissaoParaUsarForno(Personagem p);
        bool sheldonPodeUsar();
        bool amyPodeUsar();
        bool howardPodeUsar();
        bool bernardettePodeUsar();
        bool leonardPodeUsar();
        bool pennyPodeUsar();
        bool stuartPodeUsar();
        bool kripkePodeUsar();
        pthread_mutex_t forno;
        map<string, pthread_cond_t>& permissoes;
        map<string, int> esperando;
        map<string, bool> casalEsperando;
        Raj* raj;
        int contadorEspera;
        bool emUso;
};

#endif //TRABALHO_PRATICO_SO_SRC_HEADERS_FORNO_H