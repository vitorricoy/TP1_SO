#include <iostream>
#include <map>
#include <pthread.h>
#include "raj.h"

using namespace std;

Raj::Raj(map<string, int> esperando, map<string, pthread_cond_t>& permissoes) : permissoes(permissoes) {

}

void Raj::verificar() {

}