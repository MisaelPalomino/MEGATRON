#ifndef SUPERFICIE_H
#define SUPERFICIE_H

#include <string>
#include <vector>
#include "Pista.h"

using namespace std;

class Superficie {
public:
    Superficie(int id, int numPistas, int numSectores, const string& rutaBase, size_t tam) {
        string rutaSuperficie = rutaBase + "\\Superficie_" + to_string(id);
        for (int i = 0; i < numPistas; ++i) {
            pistas.emplace_back(i, rutaSuperficie, numSectores, tam);
        }
    }
    int getNumPistas(){
        return pistas.size();
    }
    Pista* getPista(int i){
        return &pistas[i];
    }
private:   
    vector<Pista> pistas;
};

#endif
