#ifndef TRAD_H
#define TRAD_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

inline bool binToTxt(const string& path_bin, const string& path_txt) {
    ifstream archivo_bin(path_bin, ios::binary);
    if (!archivo_bin.is_open()) {
        cerr << "No se pudo abrir el archivo binario: " << path_bin << endl;
        return false;
    }

    ofstream archivo_txt(path_txt);
    if (!archivo_txt.is_open()) {
        cerr << "No se pudo crear el archivo txt: " << path_txt << endl;
        return false;
    }

    unsigned char byte;
    int contador = 0;

    archivo_txt << "Contenido del archivo binario en hexadecimal:\n\n";

    while (archivo_bin.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
        archivo_txt << hex << uppercase << (int)byte << ' ';
        contador++;
        if (contador % 16 == 0) archivo_txt << '\n';  // salto de línea cada 16 bytes
    }

    archivo_bin.close();
    archivo_txt.close();

    return true;
}

#endif
