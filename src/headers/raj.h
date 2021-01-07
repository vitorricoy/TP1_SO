#ifndef TRABALHO_PRATICO_SO_SRC_HEADERS_RAJ_H
#define TRABALHO_PRATICO_SO_SRC_HEADERS_RAJ_H

#include <iostream>
#include <vector>
#include <pthread.h>

using namespace std;

class Raj {
    public:

        Raj(vector<int> esperando, vector<pthread_cond_t>& permissoes);

        void verificar();

    private:

        vector<pthread_cond_t>& permissoes;
        vector<int> esperando;
};

#endif //TRABALHO_PRATICO_SO_SRC_HEADERS_RAJ_H