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
        bool podeUsar(int codigoPersonagem);
        void atualizarCasais();
        void atualizarPrioridades();
        vector<pthread_cond_t> permissoes;
        pthread_mutex_t travaForno;
        vector<InfoPersonagemFila> personagemFila;
        vector<bool> casais;
        bool casalSheldonAmy;
        bool casalHowardBernadette;
        bool casalLeonardPenny;
        int contadorEspera;
        bool emUso;
};

#endif //TRABALHO_PRATICO_SO_SRC_HEADERS_FORNO_H