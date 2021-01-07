#include <iostream>
#include <map>
#include <pthread.h>
#include "personagem.h"
#include "raj.h"
#include "forno.h"

using namespace std;

Forno::Forno(map<string, pthread_cond_t>& permissoes, pthread_mutex_t& forno) : forno(forno), permissoes(permissoes){
    this->raj = new Raj(esperando, permissoes);
    this->esperando = map<string, int>();
    this->casalEsperando = map<string, bool>();
    this->contadorEspera = 1;
    this->emUso = false;
}

void Forno::esperar(Personagem p) {
    cout << p.getNome() << " quer usar o forno" << endl;
    this->esperando[p.getNome()] = this->contadorEspera++;
    determinarBloqueios();

    if(p.getNome() == "Sheldon") {
        this->casalEsperando["Amy"] = true;
    }

    if(p.getNome() == "Amy") {
        this->casalEsperando["Sheldon"] = true;
    }

    if(p.getNome() == "Howard") {
        this->casalEsperando["Bernadette"] = true;
    }

    if(p.getNome() == "Bernadette") {
        this->casalEsperando["Howard"] = true;
    } 

    if(p.getNome() == "Leonard") {
        this->casalEsperando["Penny"] = true;
    }

    if(p.getNome() == "Penny") {
        this->casalEsperando["Leonard"] = true;
    }

    pthread_mutex_unlock(&forno);
}

void Forno::liberar(Personagem p) {
    cout << p.getNome() << " vai comer" << endl;
    this->esperando[p.getNome()] = 0;
    determinarBloqueios();
    if(p.getNome() == "Sheldon") {
        this->casalEsperando["Amy"] = true;
    }

    if(p.getNome() == "Amy") {
        this->casalEsperando["Sheldon"] = true;
    }

    if(p.getNome() == "Howard") {
        this->casalEsperando["Bernadette"] = true;
    }

    if(p.getNome() == "Bernadette") {
        this->casalEsperando["Howard"] = true;
    } 

    if(p.getNome() == "Leonard") {
        this->casalEsperando["Penny"] = true;
    }

    if(p.getNome() == "Penny") {
        this->casalEsperando["Leonard"] = true;
    }

    emUso = false;
    pthread_mutex_unlock(&forno);
}

bool Forno::pegarForno(Personagem p) {
    if(verificarPermissaoParaUsarForno(p)) {
        emUso = true;
    }
    pthread_mutex_unlock(&forno);
    return true;
}

bool Forno::verificarPermissaoParaUsarForno(Personagem p) {
    if(emUso) {
        return false;
    }

    if(p.getNome() == "Sheldon") {
        return sheldonPodeUsar();
    }

    if(p.getNome() == "Amy") {
        return amyPodeUsar();
    }

    if(p.getNome() == "Howard") {
        return howardPodeUsar();
    }

    if(p.getNome() == "Bernadette") {
        return bernardettePodeUsar();
    } 

    if(p.getNome() == "Leonard") {
        return leonardPodeUsar();
    }

    if(p.getNome() == "Penny") {
        return pennyPodeUsar();
    }

    if(p.getNome() == "Stuart") {
        return stuartPodeUsar();
    }

    if(p.getNome() == "Kripke") {
        return kripkePodeUsar();
    }

    return false;
}

void Forno::verificar() {
    raj->verificar();
    pthread_mutex_unlock(&forno);
}

bool Forno::sheldonPodeUsar() {
    bool casalSheldonAmy = casalEsperando["Sheldon"] && casalEsperando["Amy"];
    bool casalHowardBernadette = casalEsperando["Howard"] && casalEsperando["Bernadette"];
    bool casalLeonardPenny = casalEsperando["Leonard"] && casalEsperando["Penny"];
    if(esperando["Sheldon"]) {
        if(casalLeonardPenny || casalHowardBernadette) {
            if(casalSheldonAmy) {
                if(!casalLeonardPenny) {
                    if(esperando["Sheldon"] < esperando["Amy"]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalSheldonAmy) {
                if(esperando["Sheldon"] < esperando["Amy"]) {
                    return true;
                }
            } else {
                //Nao tem casal
                if(!esperando["Leonard"] && !esperando["Penny"]) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Forno::amyPodeUsar() {
    bool casalSheldonAmy = casalEsperando["Sheldon"] && casalEsperando["Amy"];
    bool casalHowardBernadette = casalEsperando["Howard"] && casalEsperando["Bernadette"];
    bool casalLeonardPenny = casalEsperando["Leonard"] && casalEsperando["Penny"];
    if(esperando["Amy"]) {
        if(casalLeonardPenny || casalHowardBernadette) {
            if(casalSheldonAmy) {
                if(!casalLeonardPenny) {
                    if(esperando["Amy"] < esperando["Sheldon"]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalSheldonAmy) {
                if(esperando["Amy"] < esperando["Sheldon"]) {
                    return true;
                }
            } else {
                //Nao tem casal
                if(!esperando["Leonard"] && !esperando["Penny"]) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Forno::howardPodeUsar() {
    bool casalSheldonAmy = casalEsperando["Sheldon"] && casalEsperando["Amy"];
    bool casalHowardBernadette = casalEsperando["Howard"] && casalEsperando["Bernadette"];
    bool casalLeonardPenny = casalEsperando["Leonard"] && casalEsperando["Penny"];
    if(esperando["Howard"]) {
        if(casalLeonardPenny || casalSheldonAmy) {
            if(casalHowardBernadette) {
                if(!casalSheldonAmy) {
                    if(esperando["Howard"] < esperando["Bernadette"]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalHowardBernadette) {
                if(esperando["Howard"] < esperando["Bernadette"]) {
                    return true;
                }
            } else {
                //Nao tem casal
                if(!esperando["Sheldon"] && !esperando["Amy"]) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Forno::bernardettePodeUsar() {
    bool casalSheldonAmy = casalEsperando["Sheldon"] && casalEsperando["Amy"];
    bool casalHowardBernadette = casalEsperando["Howard"] && casalEsperando["Bernadette"];
    bool casalLeonardPenny = casalEsperando["Leonard"] && casalEsperando["Penny"];
    if(esperando["Bernadette"]) {
        if(casalLeonardPenny || casalSheldonAmy) {
            if(casalHowardBernadette) {
                if(!casalSheldonAmy) {
                    if(esperando["Bernadette"] < esperando["Howard"]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalHowardBernadette) {
                if(esperando["Bernadette"] < esperando["Howard"]) {
                    return true;
                }
            } else {
                //Nao tem casal
                if(!esperando["Sheldon"] && !esperando["Amy"]) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Forno::leonardPodeUsar() {
    bool casalSheldonAmy = casalEsperando["Sheldon"] && casalEsperando["Amy"];
    bool casalHowardBernadette = casalEsperando["Howard"] && casalEsperando["Bernadette"];
    bool casalLeonardPenny = casalEsperando["Leonard"] && casalEsperando["Penny"];
    if(esperando["Leonard"]) {
        if(casalHowardBernadette || casalSheldonAmy) {
            if(casalLeonardPenny) {
                if(!casalHowardBernadette) {
                    if(esperando["Leonard"] < esperando["Penny"]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalLeonardPenny) {
                if(esperando["Leonard"] < esperando["Penny"]) {
                    return true;
                }
            } else {
                //Nao tem casal
                if(!esperando["Howard"] && !esperando["Bernadette"]) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Forno::pennyPodeUsar() {
    bool casalSheldonAmy = casalEsperando["Sheldon"] && casalEsperando["Amy"];
    bool casalHowardBernadette = casalEsperando["Howard"] && casalEsperando["Bernadette"];
    bool casalLeonardPenny = casalEsperando["Leonard"] && casalEsperando["Penny"];
    if(esperando["Penny"]) {
        if(casalHowardBernadette || casalSheldonAmy) {
            if(casalLeonardPenny) {
                if(!casalHowardBernadette) {
                    if(esperando["Penny"] < esperando["Leonard"]) {
                        return true;
                    }
                }
            }
        } else {
            if(casalLeonardPenny) {
                if(esperando["Penny"] < esperando["Leonard"]) {
                    return true;
                }
            } else {
                //Nao tem casal
                if(!esperando["Howard"] && !esperando["Bernadette"]) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Forno::stuartPodeUsar() {
    bool casalSheldonAmy = casalEsperando["Sheldon"] && casalEsperando["Amy"];
    bool casalHowardBernadette = casalEsperando["Howard"] && casalEsperando["Bernadette"];
    bool casalLeonardPenny = casalEsperando["Leonard"] && casalEsperando["Penny"];
    if(esperando["Stuart"]) {
        if(!casalHowardBernadette && !casalLeonardPenny && !casalSheldonAmy) {
            if(!esperando["Sheldon"] && !esperando["Amy"] && !esperando["Howard"] && !esperando["Bernadette"] && !esperando["Leonard"] && !esperando["Penny"]) {
                return true;
            }
        }
    }
    return false;
}

bool Forno::kripkePodeUsar() {
    bool casalSheldonAmy = casalEsperando["Sheldon"] && casalEsperando["Amy"];
    bool casalHowardBernadette = casalEsperando["Howard"] && casalEsperando["Bernadette"];
    bool casalLeonardPenny = casalEsperando["Leonard"] && casalEsperando["Penny"];
    if(esperando["Kripke"]) {
        if(!casalHowardBernadette && !casalLeonardPenny && !casalSheldonAmy) {
            if(!esperando["Stuart"] && !esperando["Sheldon"] && !esperando["Amy"] && !esperando["Howard"] && !esperando["Bernadette"] && !esperando["Leonard"] && !esperando["Penny"]) {
                return true;
            }
        }
    }
    return false;
}


void Forno::determinarBloqueios() {

    //Sheldon
    if(sheldonPodeUsar()) {
        pthread_cond_signal(&permissoes["Sheldon"]);           
    }

    //Amy
    if(amyPodeUsar()) {
        pthread_cond_signal(&permissoes["Amy"]);
    }

    //Howard
    if(howardPodeUsar()) {
        pthread_cond_signal(&permissoes["Howard"]);
    }

    //Bernardette
    if(bernardettePodeUsar()) {
        pthread_cond_signal(&permissoes["Bernadette"]);
    }

    //Leonard
    if(leonardPodeUsar()) {
        pthread_cond_signal(&permissoes["Leonard"]);
    }

    //Penny
    if(pennyPodeUsar()) {
        pthread_cond_signal(&permissoes["Penny"]);
    }

    //Stuart
    if(stuartPodeUsar()) {
        pthread_cond_signal(&permissoes["Stuart"]);
    }

    //Kripke
    if(kripkePodeUsar()) {
        pthread_cond_signal(&permissoes["Kripke"]);
    }
}