#ifndef TRABALHO_PRATICO_SO_SRC_HEADERS_RAJ_H
#define TRABALHO_PRATICO_SO_SRC_HEADERS_RAJ_H

#include <iostream>
#include <map>
#include <pthread.h>

using namespace std;

class Raj {
    public:

        Raj(map<string, int> esperando, map<string, pthread_cond_t>& permissoes);

        void verificar();

    private:

        map<string, pthread_cond_t>& permissoes;
        map<string, int> esperando;
};

#endif //TRABALHO_PRATICO_SO_SRC_HEADERS_RAJ_H