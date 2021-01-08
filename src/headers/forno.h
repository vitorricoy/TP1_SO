#ifndef TRABALHO_PRATICO_SO_SRC_HEADERS_FORNO_H
#define TRABALHO_PRATICO_SO_SRC_HEADERS_FORNO_H

#include <vector>
#include <iostream>
#include <pthread.h>
#include "personagem.h"
#include "raj.h"

using namespace std;

class Forno {

    public: 

        Forno(vector<pthread_cond_t>& permissoes);

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
        void atualizarCasais();
        vector<pthread_cond_t>& permissoes;
        vector<int> esperando;
        bool casalSheldonAmy;
        bool casalHowardBernadette;
        bool casalLeonardPenny;
        Raj* raj;
        int contadorEspera;
        bool emUso;
};

#endif //TRABALHO_PRATICO_SO_SRC_HEADERS_FORNO_H