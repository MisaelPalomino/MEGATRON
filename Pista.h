#ifndef PISTA_H
#define PISTA_H

#include <string>
#include <vector>
#include "Utilidades.h"
#include "Sector.h"

using namespace std;

class Pista {
public:
    Pista(int id, const string& rutaBase, int numSectores, size_t tam) {
        string ruta = rutaBase + "\\Pista_" + to_string(id);
        crearDirectorios(ruta);
        for (int i = 0; i < numSectores; ++i) {
            string rutaSector = ruta + "\\Sector_" + to_string(i) + ".txt";
            sectores.emplace_back(rutaSector, tam);
        }

    }
    int getNumSectores(){
        return sectores.size();
    }
    Sector* getSector(int i){
        return &sectores[i];
    }
private:
    vector<Sector> sectores;
};

#endif
