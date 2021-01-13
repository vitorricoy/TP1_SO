#ifndef TRABALHO_PRATICO_SO_SRC_HEADERS_FORNO_H
#define TRABALHO_PRATICO_SO_SRC_HEADERS_FORNO_H

#include <vector>
#include <iostream>
#include <pthread.h>
#include "personagem.h"

using namespace std;

class Forno {

    public: 

        Forno();

        ~Forno();

        void esperar(Personagem p);

        void liberar(Personagem p);

        string verificar();
    
    private:
        
        // bool pegarForno(Personagem p);
        void determinarBloqueios();
        // bool verificarPermissaoParaUsarForno(Personagem p);
        bool filaVazia();
        bool sheldonPodeUsar();
        bool amyPodeUsar();
        bool howardPodeUsar();
        bool bernadettePodeUsar();
        bool leonardPodeUsar();
        bool pennyPodeUsar();
        bool stuartPodeUsar();
        bool kripkePodeUsar();
        void atualizarCasais();
        vector<pthread_cond_t> permissoes;
        pthread_mutex_t travaForno;
        vector<int> esperando;
        bool casalSheldonAmy;
        bool casalHowardBernadette;
        bool casalLeonardPenny;
        int contadorEspera;
        bool emUso;
};

#endif //TRABALHO_PRATICO_SO_SRC_HEADERS_FORNO_H