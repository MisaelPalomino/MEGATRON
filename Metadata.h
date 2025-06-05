#ifndef METADATA_H
#define METADATA_H

#include <vector>
#include <string>
#include <iostream>
#include "Disco.h"
#include "Utilidades2.h"
#include <filesystem> 
namespace fs = std::filesystem;
struct Coord{
    int plato;
    int superficie;
    int pista;
    int sector;
    Coord() : plato(0), superficie(0), pista(0), sector(0) {}
    Coord(int p, int s, int pi, int se)
        : plato(p), superficie(s), pista(pi), sector(se) {}
};
class Metadata {
private:
    vector<string> nombres;
    vector<Coord> posInicio;
    vector<Coord> posFinal;
    vector<string> referencias;
    vector<int> registrosPorSector;
    vector<int> tam_disponibles;

public:
    void agregarSector(string s) {
        referencias.push_back(s);
        fstream f(s, ios::in);
        string linea;
        getline(f, linea); 
        int tam=stoi(limpiarpalabra(linea));
        int cont = 0;
        int plato, superficie, pista, sector;
        string nombre;
        int registrosEnEsteSector = 0;

        while (getline(f, linea)) {
            if (cont % 3 == 0) {
                nombre = limpiarpalabra(linea);
                nombres.push_back(nombre);
            } else if (cont % 3 == 1) {
                cordenadas(linea, plato, superficie, pista, sector);
                posInicio.push_back(Coord(plato, superficie, pista, sector));
            } else if (cont % 3 == 2) {
                cordenadas(linea, plato, superficie, pista, sector);
                posFinal.push_back(Coord(plato, superficie, pista, sector));
                registrosEnEsteSector++;
            }
            cont++;
        }
        tam_disponibles.push_back(tam);
        registrosPorSector.push_back(registrosEnEsteSector);
    }

    string getTotal() {
        string contenido_total;
        int index = 0;
        for (int i = 0; i < referencias.size(); i++) {
            int registros = registrosPorSector[i];
            for (int j = 0; j < registros; j++) {
                contenido_total +=  nombres[index] + "\n";
                contenido_total +=  to_string(posInicio[index].plato) +
                    to_string(posInicio[index].superficie) + 
                    to_string(posInicio[index].pista) + 
                    to_string(posInicio[index].sector) + "\n";
                contenido_total +=  to_string(posFinal[index].plato) +
                    to_string(posFinal[index].superficie) + 
                    to_string(posFinal[index].pista) + 
                    to_string(posFinal[index].sector) + "\n";
                contenido_total += "\n";
                index++;
            }
        }

        return contenido_total;
    }
    string getContenido_sector_guardar(int index){
        ostringstream contenido;
        int pos = 0;
        for (int i = 0; i < index; i++) {
            pos+= registrosPorSector[i];
        }
        for(int i=0; i<registrosPorSector[index]; i++){
            contenido<< std::left << setw(16) << setfill('_') << nombres[pos] << '\n';
            contenido<< std::left << setw(10) << setfill('_') << posInicio[pos].plato
                    << setw(10) << setfill('_') << posInicio[pos].superficie
                    << setw(10) << setfill('_') << posInicio[pos].pista
                    << setw(10) << setfill('_') << posInicio[pos].sector << '\n';

            contenido<< std::left << setw(10) << setfill('_') << posFinal[pos].plato
                    << setw(10) << setfill('_') << posFinal[pos].superficie
                    << setw(10) << setfill('_') << posFinal[pos].pista
                    << setw(10) << setfill('_') << posFinal[pos].sector << '\n';
            pos++;
        }
        return contenido.str();
    }

    Coord getCordsIni(int i) { return posInicio[i]; }
    Coord getCordsFin(int i) { return posFinal[i]; }
    string getNombre(int i) { return nombres[i]; }
    string getReferencia(int i){ return referencias[i]; }
    int cantidad_sectores() { return referencias.size(); }
    int get_cant_reg_Sector(int i){return registrosPorSector[i];}
    void setNombre(int i, string s){nombres[i]=s;}
    int get_tam_disponible(int i){return tam_disponibles[i];}
};


#endif
