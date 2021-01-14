#ifndef TRABALHO_PRATICO_SO_SRC_HEADERS_CONSTANTES_H
#define TRABALHO_PRATICO_SO_SRC_HEADERS_CONSTANTES_H

#include <iostream>

using namespace std;

namespace Constantes {
    // Códigos não podem mudar, devido ao tratamento aritmético feito aos casais
    const int SHELDON = 0;
    const int AMY = 1;
    const int HOWARD = 2;
    const int BERNADETTE = 3;
    const int LEONARD = 4;
    const int PENNY = 5;
    const int STUART = 6;
    const int KRIPKE = 7;

    const int NUMERO_PERSONAGENS = 8;
    const int NUMERO_CASAIS = 3;

    const string NOME_SHELDON = "Sheldon";
    const string NOME_AMY = "Amy";
    const string NOME_HOWARD = "Howard";
    const string NOME_BERNADETTE = "Bernadette";
    const string NOME_LEONARD = "Leonard";
    const string NOME_PENNY = "Penny";
    const string NOME_STUART = "Stuart";
    const string NOME_KRIPKE = "Kripke";

    const int FORA_FILA = 0;
    const int SOZINHO_FILA = 1;
    const int CASAL_DESFEITO_FILA = 2;
    const int CASAL_FILA = 3;
}


#endif //TRABALHO_PRATICO_SO_SRC_HEADERS_CONSTANTES_H