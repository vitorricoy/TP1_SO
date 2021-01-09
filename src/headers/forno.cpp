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
    determinarBloqueios();
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
    cout << p.getNome() << " vai comer" << endl;
    this->esperando[p.getCodigo()] = 0;
    emUso = false;
    determinarBloqueios();
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

void Forno::verificar() {
    pthread_mutex_lock(&this->travaForno);
    pthread_mutex_unlock(&this->travaForno);
}

bool Forno::sheldonPodeUsar() {
    if(esperando[Constantes::SHELDON]) {
        if(casalLeonardPenny || casalHowardBernadette) {
            if(casalSheldonAmy) {
                if(!casalLeonardPenny) {
                    if(esperando[Constantes::SHELDON] < esperando[Constantes::AMY]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalSheldonAmy) {
                if(esperando[Constantes::SHELDON] < esperando[Constantes::AMY]) {
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
                    if(esperando[Constantes::AMY] < esperando[Constantes::SHELDON]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalSheldonAmy) {
                if(esperando[Constantes::AMY] < esperando[Constantes::SHELDON]) {
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
                    if(esperando[Constantes::HOWARD] < esperando[Constantes::BERNADETTE]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalHowardBernadette) {
                if(esperando[Constantes::HOWARD] < esperando[Constantes::BERNADETTE]) {
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
                    if(esperando[Constantes::BERNADETTE] < esperando[Constantes::HOWARD]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalHowardBernadette) {
                if(esperando[Constantes::BERNADETTE] < esperando[Constantes::HOWARD]) {
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
                    if(esperando[Constantes::LEONARD] < esperando[Constantes::PENNY]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalLeonardPenny) {
                if(esperando[Constantes::LEONARD] < esperando[Constantes::PENNY]) {
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
                    if(esperando[Constantes::PENNY] < esperando[Constantes::LEONARD]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalLeonardPenny) {
                if(esperando[Constantes::PENNY] < esperando[Constantes::LEONARD]) {
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

void Forno::determinarBloqueios() {

    if(emUso) {
        return;
    }

    atualizarCasais();

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