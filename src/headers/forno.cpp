#include <iostream>
#include <vector>
#include <pthread.h>
#include "personagem.h"
#include "forno.h"
#include "constantes.h"

using namespace std;

Forno::Forno(){
    this->permissoes = vector<pthread_cond_t>(Constantes::NUMERO_PERSONAGENS);
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        pthread_cond_init(&this->permissoes[I], NULL);
    }
    pthread_mutex_init(&this->travaForno, NULL);
    this->esperando = vector<int>(Constantes::NUMERO_PERSONAGENS);
    this->casais = vector<bool>(Constantes::NUMERO_CASAIS);
    this->casalSheldonAmy = false;
    this->casalHowardBernadette = false;
    this->casalLeonardPenny = false;
    this->contadorEspera = 1;
    this->emUso = false;
}

Forno::~Forno() {
    for(int I=0; I<Constantes::NUMERO_PERSONAGENS; I++) {
        pthread_cond_destroy(&this->permissoes[I]);
    }
    pthread_mutex_destroy(&this->travaForno);
}

void Forno::esperar(Personagem p) {
    pthread_mutex_lock(&this->travaForno);
    cout << p.getNome() << " quer usar o forno" << endl;
    this->esperando[p.getCodigo()] = this->contadorEspera++;
    this->determinarBloqueios();
    bool jaEstaLiberado = false;
    switch(p.getCodigo()) {
        case Constantes::SHELDON: jaEstaLiberado = sheldonPodeUsar(); break; 
        case Constantes::AMY: jaEstaLiberado = amyPodeUsar(); break; 
        case Constantes::HOWARD: jaEstaLiberado = howardPodeUsar(); break; 
        case Constantes::BERNADETTE: jaEstaLiberado = bernadettePodeUsar(); break; 
        case Constantes::LEONARD: jaEstaLiberado = leonardPodeUsar(); break; 
        case Constantes::PENNY: jaEstaLiberado = pennyPodeUsar(); break; 
        case Constantes::STUART: jaEstaLiberado = stuartPodeUsar(); break; 
        case Constantes::KRIPKE: jaEstaLiberado = kripkePodeUsar(); break;
    }
    if(!emUso || !jaEstaLiberado) {
        pthread_cond_wait(&this->permissoes[p.getCodigo()], &this->travaForno);
    }
    emUso = true;
    pthread_mutex_unlock(&this->travaForno);
}

void Forno::liberar(Personagem p) {
    pthread_mutex_lock(&this->travaForno);
    this->esperando[p.getCodigo()] = 0;
    emUso = false;
    this->determinarBloqueios();
    pthread_mutex_unlock(&this->travaForno);
}

// bool Forno::pegarForno(Personagem p) {
//     if(verificarPermissaoParaUsarForno(p)) {
//         emUso = true;
//     }
//     return true;
// }

// bool Forno::verificarPermissaoParaUsarForno(Personagem p) {
//     if(emUso) {
//         return false;
//     }

//     if(p.getCodigo() == Constantes::SHELDON) {
//         return sheldonPodeUsar();
//     }

//     if(p.getCodigo() == Constantes::AMY) {
//         return amyPodeUsar();
//     }

//     if(p.getCodigo() == Constantes::HOWARD) {
//         return howardPodeUsar();
//     }

//     if(p.getCodigo() == Constantes::BERNADETTE) {
//         return bernadettePodeUsar();
//     } 

//     if(p.getCodigo() == Constantes::LEONARD) {
//         return leonardPodeUsar();
//     }

//     if(p.getCodigo() == Constantes::PENNY) {
//         return pennyPodeUsar();
//     }

//     if(p.getCodigo() == Constantes::STUART) {
//         return stuartPodeUsar();
//     }

//     if(p.getCodigo() == Constantes::KRIPKE) {
//         return kripkePodeUsar();
//     }

//     return false;
// }

bool Forno::filaVazia() {
    for(int elemento: this->esperando) {
        if(elemento) {
            return false;
        }
    }
    return true;
}

string Forno::verificar() {
    pthread_mutex_lock(&this->travaForno);
    string retorno = "";

    if(!this->filaVazia() && !this->emUso) {
        if(!this->sheldonPodeUsar() && !this->amyPodeUsar() && !this->howardPodeUsar() && !this->bernadettePodeUsar() 
           && !this->leonardPodeUsar() && !this->pennyPodeUsar() && !this->stuartPodeUsar() && !this->kripkePodeUsar()) {
            int escolhido = drand48()*3;
            switch(escolhido) {
                case 0: 
                    if(casalSheldonAmy && esperando[Constantes::SHELDON] && esperando[Constantes::AMY]) { // Tratando o caso de apenas um dos membros do casal estar na fila com prioridade de casal
                        if(esperando[Constantes::SHELDON] < esperando[Constantes::AMY]) {
                            pthread_cond_signal(&permissoes[Constantes::SHELDON]);
                            retorno = Constantes::NOME_SHELDON;
                        } else {
                            pthread_cond_signal(&permissoes[Constantes::AMY]);
                            retorno = Constantes::NOME_AMY;
                        }
                    } else if(esperando[Constantes::SHELDON]) {
                            pthread_cond_signal(&permissoes[Constantes::SHELDON]);
                            retorno = Constantes::NOME_SHELDON;
                    } else { // AMY esperando
                            pthread_cond_signal(&permissoes[Constantes::AMY]);
                            retorno = Constantes::NOME_AMY;
                    }
                    break;
                case 1:
                    if(casalHowardBernadette && esperando[Constantes::HOWARD] && esperando[Constantes::BERNADETTE]) { // Tratando o caso de apenas um dos membros do casal estar na fila com prioridade de casal
                        if(esperando[Constantes::HOWARD] < esperando[Constantes::BERNADETTE]) {
                            pthread_cond_signal(&permissoes[Constantes::HOWARD]);
                            retorno = Constantes::NOME_HOWARD;
                        } else {
                            pthread_cond_signal(&permissoes[Constantes::BERNADETTE]);
                            retorno = Constantes::NOME_BERNADETTE;
                        }
                    } else if(esperando[Constantes::HOWARD]) {
                            pthread_cond_signal(&permissoes[Constantes::HOWARD]);
                            retorno = Constantes::NOME_HOWARD;
                    } else { // BERNADETTE esperando
                            pthread_cond_signal(&permissoes[Constantes::BERNADETTE]);
                            retorno = Constantes::NOME_BERNADETTE;
                    }
                    break;
                case 2:
                    if(casalLeonardPenny && esperando[Constantes::LEONARD] && esperando[Constantes::PENNY]) { // Tratando o caso de apenas um dos membros do casal estar na fila com prioridade de casal
                        if(esperando[Constantes::LEONARD] < esperando[Constantes::PENNY]) {
                            pthread_cond_signal(&permissoes[Constantes::LEONARD]);
                            retorno = Constantes::NOME_LEONARD;
                        } else {
                            pthread_cond_signal(&permissoes[Constantes::PENNY]);
                            retorno = Constantes::NOME_PENNY;
                        }
                    } else if(esperando[Constantes::LEONARD]) {
                            pthread_cond_signal(&permissoes[Constantes::LEONARD]);
                            retorno = Constantes::NOME_LEONARD;
                    } else { // PENNY esperando
                            pthread_cond_signal(&permissoes[Constantes::PENNY]);
                            retorno = Constantes::NOME_PENNY;
                    }
                    break;
            }
        }
    }
    pthread_mutex_unlock(&this->travaForno);
    return retorno;
}

bool Forno::sheldonPodeUsar() {
    if(esperando[Constantes::SHELDON]) {
        if(casalLeonardPenny || casalHowardBernadette) {
            if(casalSheldonAmy) {
                if(!casalLeonardPenny) {
                    if(esperando[Constantes::SHELDON] < esperando[Constantes::AMY] || !esperando[Constantes::AMY]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalSheldonAmy) {
                if(esperando[Constantes::SHELDON] < esperando[Constantes::AMY] || !esperando[Constantes::AMY]) {
                    return true;
                }
            } else {
                //Nao tem casal
                if(!esperando[Constantes::LEONARD] && !esperando[Constantes::PENNY]) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Forno::amyPodeUsar() {
    if(esperando[Constantes::AMY]) {
        if(casalLeonardPenny || casalHowardBernadette) {
            if(casalSheldonAmy) {
                if(!casalLeonardPenny) {
                    if(esperando[Constantes::AMY] < esperando[Constantes::SHELDON] || !esperando[Constantes::SHELDON]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalSheldonAmy) {
                if(esperando[Constantes::AMY] < esperando[Constantes::SHELDON] || !esperando[Constantes::SHELDON]) {
                    return true;
                }
            } else {
                //Nao tem casal
                if(!esperando[Constantes::LEONARD] && !esperando[Constantes::PENNY]) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Forno::howardPodeUsar() {
    if(esperando[Constantes::HOWARD]) {
        if(casalLeonardPenny || casalSheldonAmy) {
            if(casalHowardBernadette) {
                if(!casalSheldonAmy) {
                    if(esperando[Constantes::HOWARD] < esperando[Constantes::BERNADETTE] || !esperando[Constantes::BERNADETTE]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalHowardBernadette) {
                if(esperando[Constantes::HOWARD] < esperando[Constantes::BERNADETTE] || !esperando[Constantes::BERNADETTE]) {
                    return true;
                }
            } else {
                //Nao tem casal
                if(!esperando[Constantes::SHELDON] && !esperando[Constantes::AMY]) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Forno::bernadettePodeUsar() {
    if(esperando[Constantes::BERNADETTE]) {
        if(casalLeonardPenny || casalSheldonAmy) {
            if(casalHowardBernadette) {
                if(!casalSheldonAmy) {
                    if(esperando[Constantes::BERNADETTE] < esperando[Constantes::HOWARD] || !esperando[Constantes::HOWARD]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalHowardBernadette) {
                if(esperando[Constantes::BERNADETTE] < esperando[Constantes::HOWARD] || !esperando[Constantes::HOWARD]) {
                    return true;
                }
            } else {
                //Nao tem casal
                if(!esperando[Constantes::SHELDON] && !esperando[Constantes::AMY]) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Forno::leonardPodeUsar() {
    if(esperando[Constantes::LEONARD]) {
        if(casalHowardBernadette || casalSheldonAmy) {
            if(casalLeonardPenny) {
                if(!casalHowardBernadette) {
                    if(esperando[Constantes::LEONARD] < esperando[Constantes::PENNY] || !esperando[Constantes::PENNY]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalLeonardPenny) {
                if(esperando[Constantes::LEONARD] < esperando[Constantes::PENNY] || !esperando[Constantes::PENNY]) {
                    return true;
                }
            } else {
                //Nao tem casal
                if(!esperando[Constantes::HOWARD] && !esperando[Constantes::BERNADETTE]) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Forno::pennyPodeUsar() {
    if(esperando[Constantes::PENNY]) {
        if(casalHowardBernadette || casalSheldonAmy) {
            if(casalLeonardPenny) {
                if(!casalHowardBernadette) {
                    if(esperando[Constantes::PENNY] < esperando[Constantes::LEONARD] || !esperando[Constantes::LEONARD]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalLeonardPenny) {
                if(esperando[Constantes::PENNY] < esperando[Constantes::LEONARD] || !esperando[Constantes::LEONARD]) {
                    return true;
                }
            } else {
                //Nao tem casal
                if(!esperando[Constantes::HOWARD] && !esperando[Constantes::BERNADETTE]) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Forno::stuartPodeUsar() {
    if(esperando[Constantes::STUART]) {
        if(!casalHowardBernadette && !casalLeonardPenny && !casalSheldonAmy) {
            if(!esperando[Constantes::SHELDON] && !esperando[Constantes::AMY] && !esperando[Constantes::HOWARD] && !esperando[Constantes::BERNADETTE] && !esperando[Constantes::LEONARD] && !esperando[Constantes::PENNY]) {
                return true;
            }
        }
    }
    return false;
}

bool Forno::kripkePodeUsar() {
    if(esperando[Constantes::KRIPKE]) {
        if(!casalHowardBernadette && !casalLeonardPenny && !casalSheldonAmy) {
            if(!esperando[Constantes::STUART] && !esperando[Constantes::SHELDON] && !esperando[Constantes::AMY] && !esperando[Constantes::HOWARD] && !esperando[Constantes::BERNADETTE] && !esperando[Constantes::LEONARD] && !esperando[Constantes::PENNY]) {
                return true;
            }
        }
    }
    return false;
}

//Considera que o privilegio de casal acaba somente quanto ambos usam o forno e saem da fila
void Forno::atualizarCasais() {

    for(int I=0; I<Constantes::NUMERO_CASAIS; I++) {
        if(this->casais[I]) {
            this->casais[I] = esperando[2*I] || esperando[2*I + 1];
        } else {
            this->casais[I] = esperando[2*I] && esperando[2*I + 1];
        }
    }

    if(casalSheldonAmy) {
        casalSheldonAmy = esperando[Constantes::SHELDON] || esperando[Constantes::AMY];
    } else {
        casalSheldonAmy = esperando[Constantes::SHELDON] && esperando[Constantes::AMY];
    }

    if(casalHowardBernadette) {
        casalHowardBernadette = esperando[Constantes::HOWARD] || esperando[Constantes::BERNADETTE];
    } else {
        casalHowardBernadette = esperando[Constantes::HOWARD] && esperando[Constantes::BERNADETTE];
    }

    if(casalLeonardPenny) {
        casalLeonardPenny = esperando[Constantes::LEONARD] || esperando[Constantes::PENNY];
    } else {
        casalLeonardPenny = esperando[Constantes::LEONARD] && esperando[Constantes::PENNY];
    }
}

bool Forno::podeUsar(int codigoPersonagem) {
    if(this->esperando[codigoPersonagem]) {
        auto modPositivo = [](int x, int mod) { return ((x%mod)+mod)%mod; };
        if(codigoPersonagem <= 5) { // Personagem pode ser membro de casal
            int codigoCasalMaiorPrioridade = modPositivo(codigoPersonagem/2-1, Constantes::NUMERO_CASAIS);
            int codigoCasalAtual = codigoPersonagem/2;
            bool existeCasal = false;
            for(bool el : this->casais) {
                existeCasal = existeCasal || el;
            }
            bool casalPresente = this->casais[codigoCasalAtual]; // Obtém se o casal do personagem está presente
            if(existeCasal && casalPresente) {
                if(!this->casais[codigoCasalMaiorPrioridade]) { // Verifica o casal anterior na lista de prioridades (que tem mais prioridade)
                    if(codigoPersonagem%2 == 0) { // É o membro masculino do casal
                        if(this->esperando[codigoPersonagem] < this->esperando[codigoPersonagem+1] || !this->esperando[codigoPersonagem+1]) {
                            return true;
                        }
                    } else { // É o membro feminino
                        if(this->esperando[codigoPersonagem] < this->esperando[codigoPersonagem-1] || !this->esperando[codigoPersonagem-1]) {
                            return true;
                        }
                    }
                }
            } else {
                if(!existeCasal) { // Não tem casal
                    if(!this->esperando[codigoCasalMaiorPrioridade*2] && !this->esperando[codigoCasalMaiorPrioridade*2 + 1]) { // Verifica se nenhum personagem com mais prioridade está esperando na fila
                        return true;
                    }
                }
            }
        } else { // Stuart e Kripke
            bool algumPrioritarioEsperando = false;
            for(int I=0; I<codigoPersonagem; I++) {
                algumPrioritarioEsperando = algumPrioritarioEsperando || this->esperando[I];
            }
            if(!algumPrioritarioEsperando) {
                return true;
            }
        }
    }
    return false;
}

void Forno::determinarBloqueios() {

    if(emUso) {
        return;
    }

    atualizarCasais();

    if(sheldonPodeUsar() != podeUsar(0) || amyPodeUsar() != podeUsar(1) || howardPodeUsar() != podeUsar(2) || bernadettePodeUsar() != podeUsar(3) || leonardPodeUsar() != podeUsar(4) ||
       pennyPodeUsar() != podeUsar(5) || stuartPodeUsar() != podeUsar(6) || kripkePodeUsar() != podeUsar(7)) {
           cout << "Erro!!" << endl << "Esperando: ";
           for(int el : this->esperando) {
               cout << el << " ";
           }
           cout << endl << " " << "Podem usar: " << endl;
           cout << sheldonPodeUsar() << " " << podeUsar(0) << endl;
           cout << amyPodeUsar() << " " << podeUsar(1) << endl;
           cout << howardPodeUsar() << " " << podeUsar(2) << endl;
           cout << bernadettePodeUsar() << " " << podeUsar(3) << endl;
           cout << leonardPodeUsar() << " " << podeUsar(4) << endl;
           cout << pennyPodeUsar() << " " << podeUsar(5) << endl;
           cout << stuartPodeUsar() << " " << podeUsar(6) << endl;
           cout << kripkePodeUsar() << " " << podeUsar(7) << endl << endl;
       }

    //Sheldon
    if(sheldonPodeUsar()) {
        pthread_cond_signal(&permissoes[Constantes::SHELDON]);           
    }

    //Amy
    if(amyPodeUsar()) {
        pthread_cond_signal(&permissoes[Constantes::AMY]);
    }

    //Howard
    if(howardPodeUsar()) {
        pthread_cond_signal(&permissoes[Constantes::HOWARD]);
    }

    //Bernardette
    if(bernadettePodeUsar()) {
        pthread_cond_signal(&permissoes[Constantes::BERNADETTE]);
    }

    //Leonard
    if(leonardPodeUsar()) {
        pthread_cond_signal(&permissoes[Constantes::LEONARD]);
    }

    //Penny
    if(pennyPodeUsar()) {
        pthread_cond_signal(&permissoes[Constantes::PENNY]);
    }

    //Stuart
    if(stuartPodeUsar()) {
        pthread_cond_signal(&permissoes[Constantes::STUART]);
    }

    //Kripke
    if(kripkePodeUsar()) {
        pthread_cond_signal(&permissoes[Constantes::KRIPKE]);
    }
}