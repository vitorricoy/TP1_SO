#ifndef TRABALHO_PRATICO_SO_SRC_HEADERS_INFO_PERSONAGEM_FILA_H
#define TRABALHO_PRATICO_SO_SRC_HEADERS_INFO_PERSONAGEM_FILA_H

#include <iostream>

using namespace std;

class InfoPersonagemFila {
    public:

        InfoPersonagemFila(); // Construtor

        // Define a prioridade do personagem na fila, de acordo com os códigos definidos nas constantes
        void setPrioridade(int prioridade);
        // Define o código do personagem que o objeto representa
        void setCodigo(int codigo);

        void entrarNaFila(int tempoChegada); // Determina que o personagem entra na fila no tempo especificado
        void sairDaFila(); // Determina que o personagem saiu da fila
        void usarForno(); // Determina que o personagem está usando o forno
        void liberarForno(); // Determina que o personagem liberou o forno

        bool estaNaFila(); // Verifica se o personagem está na fila
        bool estaUsandoForno(); // Verifica se o personagem está usando o forno
        int getTempoChegada(); // Obtém o tempo de chegada do personagem na fila
        int getPrioridade(); // Obtém a prioridade do personagem na fila
        int getCodigo();  // Obtém o código do personagem que o objeto representa

        // Determina se o personagem tem mais prioridade do que o personagem enviado por argumento
        // Cas os dois 'empatem' é retornado false para ambos
        bool temMaisPrioridade(InfoPersonagemFila comparacao);

    private:

        int tempoChegada; // Momento que o personagem chegou na fila (0 caso não esteja na fila)
        int nivelPrioridade; // Nível de prioridade superior do personagem
        bool presenteNaFila; // Flag para indicar se o personagem está na fila
        bool usandoForno; // Flag para indicar se o personagem está usando o forno
        int codigoPersonagem; // Código do personagem
};

#endif //TRABALHO_PRATICO_SO_SRC_HEADERS_INFO_PERSONAGEM_FILA_H