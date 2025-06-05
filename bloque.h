#ifndef BLOQUE_H
#define BLOQUE_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "Disco.h"
#include <filesystem> 
namespace fs = std::filesystem;

class Bloque {
private:
    vector<string> contenido;       
    vector<string> referencias;       
    size_t espacio_disponible = 0;
    string cabecera;
public:
    Bloque() = default;

    void agregarSector(const string& s) {
        referencias.push_back(s);
        fstream f(s, ios::in);

        if (!f.is_open()) {
            cerr << "Error: no se pudo abrir el archivo: " << s << endl;
            return;
        }

        string contenido_sector;
        if (referencias.size() == 1) {
            string d;
            char c;
            while (f.get(c)) {
                if (c == '\n') break;
                if (c != '_') d += c;
            }

            if (d.empty()) {
                cerr << "Advertencia: encabezado vacío o inválido en el sector " << s << endl;
                d = "0";
            }

            try {
                espacio_disponible = stoi(d);
            } catch (const exception& e) {
                cerr << "Error al convertir disponibilidad del bloque: " << e.what() << endl;
                espacio_disponible = 0;
            }
            string cab;
            while(f.get(c)){
                if(c=='\n') break;
                cab+=c;
            }
            cabecera=cab;
        }

        char c;
        while (f.get(c)) {
            contenido_sector += c;
        }

        contenido.push_back(contenido_sector);
        f.close();
    }

    string getContenido(int i) const {
        return contenido[i];
    }

    string getContenidoTotal() {
        string s;
        for (const auto& c : contenido) s += c;
        return s;
    }

    void limpiar() {
        contenido.clear();
        referencias.clear();
        espacio_disponible = 0;
    }

    string getSector(int i) {
        return referencias[i];
    }

    int get_espacioDisponible() {
        return espacio_disponible;
    }

    void set_espacioDisponible(int nuevo) {
        espacio_disponible = nuevo;
    }

    void set_contenido(string cont, int i) {
        contenido[i] = cont;
    }

    int getCantidad_Sectores() {
        return referencias.size();
    }

    void addContenido(int i, const string& s) {
        contenido[i] += s;
    }

    void exportarTXTbloque(int i, const string& tabla) {
        string archivo_salida = "D:/BD/Megatron3000_1/Bloque/" + tabla + to_string(i) + ".txt";
        ofstream archivo(archivo_salida);
        if (!archivo.is_open()) {
            cerr << "Error al abrir archivo de salida: " << archivo_salida << endl;
            return;
        }

        archivo << setw(12) << setfill('_') << espacio_disponible;
        archivo.put('\n');
        archivo << cabecera <<endl;
        for (const auto& c : contenido) {
            archivo << c;
        }

        archivo.close();
    }
    void imprimirBloque(){
        cout << setw(12) << setfill('_') << espacio_disponible;
        cout <<'\n';
        cout << cabecera <<endl;
        cout << '\n';
        for (const auto& c : contenido) {
            cout << c;
        }
        cout << endl;
    }

    string get_cabecera(){ return cabecera;}
    void set_cabecera(const string& s) { cabecera = s; }
    string getContenido_en_posicion(int pos, int tam) {
        int acumulado = 0;
        for (const auto& sector : contenido) {
            if (acumulado + (int)(sector.size()) >= (pos + 1) * tam) {
                return sector.substr(pos * tam - acumulado, tam);
            }
            acumulado += sector.size();
        }
        return "";
    }
    void setContenido_en_posicion(int pos, const string& nuevo, int tam, int &sector_idx) {
        int acumulado = 0;
        for (int i = 0; i < contenido.size(); ++i) {
            if (acumulado + (int)(contenido[i].size()) >= (pos + 1) * tam) {
                int local_pos = pos * tam - acumulado;
                contenido[i].replace(local_pos, tam, nuevo);
                sector_idx = i; 
                return;
            }
            acumulado += contenido[i].size();
        }
        sector_idx = -1; 
    }
    void insertarEnCabecera(size_t pos, const string& str) {
        if (cabecera.size() < pos + str.size()) {
            // Rellenar con '_' si la cabecera es más corta
            cabecera.resize(pos + str.size(), '_');
        }

        // Reemplazar el contenido en la posición deseada
        cabecera.replace(pos, str.size(), str);
    }
    string& getContenidoRef(int sector) {
        if (sector < 0 || sector >= (int)contenido.size()) {
            throw out_of_range("Sector inválido");
        }
        return contenido[sector];
    }
};

#endif
