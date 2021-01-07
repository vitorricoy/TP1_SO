#include <iostream>
#include <vector>
#include <pthread.h>
#include "raj.h"

using namespace std;

Raj::Raj(vector<int> esperando, vector<pthread_cond_t>& permissoes) : permissoes(permissoes) {

}

void Raj::verificar() {

}