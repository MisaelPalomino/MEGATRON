#ifndef PLATO_H
#define PLATO_H

#include <string>
#include <vector>
#include "Superficie.h"

using namespace std;

class Plato {
public:
    Plato(int id, int numSuperficies, int numPistas, int numSectores, const string& rutaBase, size_t tam) {
        string rutaPlato = rutaBase + "\\Plato_" + to_string(id);
        for (int i = 0; i < numSuperficies; ++i) {
            superficies.emplace_back(i, numPistas, numSectores, rutaPlato, tam);
        }
    }
    int getNumSuperficies(){
        return superficies.size();
    }
    Superficie* getSuperficie(int i){
        return &superficies[i];
    }

private:
    vector<Superficie> superficies;
};

#endif
