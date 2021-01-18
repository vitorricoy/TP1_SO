#ifndef TRABALHO_PRATICO_SO_SRC_HEADERS_INFO_PERSONAGEM_FILA_H
#define TRABALHO_PRATICO_SO_SRC_HEADERS_INFO_PERSONAGEM_FILA_H

#include <iostream>

using namespace std;

class InfoPersonagemFila {
    public:

        InfoPersonagemFila();

        void setPrioridade(int prioridade); // Define a prioridade do personagem na fila: 1- Sozinho, 2- Membro de um casal desfeito, 3- Membro de um casal
        void setCodigo(int codigo);

        void entrarNaFila(int tempoChegada); // Entra na fila no tempo especificada
        void sairDaFila(); // Sai da fila
        void usarForno();
        void liberarForno();

        bool estaNaFila(); // Verifica se está na fila
        bool estaUsandoForno();
        int getTempoChegada(); // Obtém o tempo de chegada na fila
        int getPrioridade(); // Obtém a prioridade na fila
        int getCodigo();

        bool temMaisPrioridade(InfoPersonagemFila comparacao);

    private:
        int tempoChegada;
        int nivelPrioridade;
        bool presenteNaFila;
        bool usandoForno;
        int codigoPersonagem;
};

#endif //TRABALHO_PRATICO_SO_SRC_HEADERS_INFO_PERSONAGEM_FILA_H