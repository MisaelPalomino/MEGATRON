#ifndef SECTOR_H
#define SECTOR_H

#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Sector {
private:
    string path_archivo;
    size_t tam_sector;

public:
    Sector(const string& path, int tam) : path_archivo(path), tam_sector(tam) {
        ofstream archivo(path_archivo, ios::app);
        archivo.seekp(0, ios::end);
        archivo.close();
    }
    
    string leerDatos() const {
        ifstream archivo(path_archivo);
        if (!archivo.is_open()) return "";

        archivo.seekg(0, ios::end);
        int tam_datos = archivo.tellg();
        archivo.seekg(0, ios::beg);

        string contenido(tam_datos, '\0');
        archivo.read(&contenido[0], tam_datos);
        archivo.close();
        return contenido;
    }
    
    ifstream leer_datos() const {
        return ifstream(path_archivo);
    }

    int getTamSector() const {
        return tam_sector;
    }
    string getPath() const {
        return path_archivo;
    }
};

#endif
