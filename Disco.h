#ifndef DISCO_H
#define DISCO_H

#include <vector>
#include <string>
#include "Plato.h"

using namespace std;

class Disco {
private:
    int numPlatos;
    int numSuperficies;
    int numPistas;
    int numSectores;
    int sectoresPorBloque;
    int cantidad_bloques;
    size_t capacidad_disco_bytes;
    size_t capacidad_disponible;
    size_t tam_sector;
    vector<Plato> platos;

public:
    Disco(){}
    Disco(int numPlatos, int numSuperficies, int numPistas, int numSectores, size_t tam, int SectPorBloq)
        : numPlatos(numPlatos), numSuperficies(numSuperficies),
          numPistas(numPistas), numSectores(numSectores), tam_sector(tam) , sectoresPorBloque(SectPorBloq) {
        string rutaBase = "Disco";
        for (int i = 0; i < numPlatos; ++i) {
            platos.emplace_back(i, numSuperficies, numPistas, numSectores, rutaBase, tam);
        }
        int cantidad_bloques_inc=numPlatos*numSuperficies*numPistas*numSectores;
        cantidad_bloques=cantidad_bloques_inc/sectoresPorBloque;
        if(cantidad_bloques_inc%sectoresPorBloque>0){
            cantidad_bloques++;
        }
    }

    int getNumPlatos() const {
        return numPlatos;
    }

    int getNumSuperficies() const {
        return numSuperficies;
    }

    int getNumPistas() const {
        return numPistas;
    }

    int getNumSectores() const {
        return numSectores;
    }

    size_t getTamSector() const {
        return tam_sector;
    }

    Plato* getPlato(int i) {
        return &platos[i];
    }

    void capacidad_del_disco(){
        float cap=numPlatos*numSuperficies*numPistas*numSectores*tam_sector;
        capacidad_disco_bytes=cap;
        capacidad_disponible=cap;
        float MB=cap/1024;
        MB/=1024;
        cout<<"Capacidad en bytes: "<<capacidad_disco_bytes<<'\n';
        cout<<"Capacidad en MB: "<<MB<<'\n';
        cout<<"Capacidad_disponible: "<<capacidad_disponible<<'\n';
        cout<<"Cantidad total de bloques: "<<cantidad_bloques<<'\n';
    }
    int getSectoresPorBloque() {
        return sectoresPorBloque;
    }
    void mostrar_arbol() {
        cout << "\n===== ARBOL DE CREACION DEL DISCO =====\n";
        for (int i = 0; i < numPlatos; ++i) {
            cout << "Plato " << i << ":\n";
            for (int j = 0; j < numSuperficies; ++j) {
                cout << "   Superficie " << j << ":\n";
                for (int k = 0; k < numPistas; ++k) {
                    cout << "     Pista " << k << ":\n";
                    for (int l = 0; l < numSectores; ++l) {
                        cout << "       Sector " << l << '\n';
                    }
                }
            }
        }
    }
    void actualizar_tamano(size_t n){
        capacidad_disponible=n;
    }
    size_t Get_Capacidad_Disco(){
        return capacidad_disponible;
    }
    void mostrar_estado_actual(){
        cout<<"Capacidad total en bytes: "<<capacidad_disco_bytes<<endl;
        cout<<"Capacidad Disponible: "<<capacidad_disponible<<endl;
        cout<<"Capacidad Ocupada: "<<capacidad_disco_bytes-capacidad_disponible<<endl;
    }
    int get_cantidad_bloques(){
        return cantidad_bloques;
    }
};

#endif

