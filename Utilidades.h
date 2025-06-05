#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <string>
#include <vector>
#include <windows.h>
#include <iostream>
#include <cstdint> 
#include <fstream>

using namespace std;

bool crearDirectorios(const string& ruta) {
    string directorio = "";
    vector<string> partes;

    size_t inicio = 0;
    while (inicio < ruta.size()) {
        size_t fin = ruta.find('\\', inicio);
        if (fin == string::npos) fin = ruta.size();
        string parte = ruta.substr(inicio, fin - inicio);
        if (!parte.empty()) partes.push_back(parte);
        inicio = fin + 1;
    }

    for (const string& parte : partes) {
        if (!directorio.empty()) directorio += "\\";
        directorio += parte;

        if (!CreateDirectoryA(directorio.c_str(), NULL)) {
            DWORD error = GetLastError();
            if (error != ERROR_ALREADY_EXISTS) {
                cerr << "Error al crear el directorio: " << directorio << "\n";
                return false;
            }
        }
    }

    return true;
}

void extraerCabeceraBinATxt(const std::string& archivoBin, const std::string& archivoTxt) {
    std::ifstream input(archivoBin, std::ios::binary);
    if (!input) {
        std::cerr << "No se pudo abrir el archivo binario: " << archivoBin << std::endl;
        return;
    }

    // Leer los primeros 5 bytes
    char cabecera[5];
    input.read(cabecera, 5);
    if (input.gcount() < 5) {
        std::cerr << "El archivo es muy pequeño para contener una cabecera completa.\n";
        return;
    }

    input.close();

    // Interpretar bytes
    uint8_t indicador = static_cast<uint8_t>(cabecera[0]);

    // Leer capacidad disponible como uint32_t desde bytes [1..4]
    // Suponiendo formato little endian:
    uint32_t capacidadDisponible = 
        static_cast<uint8_t>(cabecera[1]) |
        (static_cast<uint8_t>(cabecera[2]) << 8) |
        (static_cast<uint8_t>(cabecera[3]) << 16) |
        (static_cast<uint8_t>(cabecera[4]) << 24);

    // Escribir a archivo texto
    std::ofstream output(archivoTxt);
    if (!output) {
        std::cerr << "No se pudo abrir el archivo de salida: " << archivoTxt << std::endl;
        return;
    }

    output << "Cabecera del archivo binario: " << archivoBin << "\n";
    output << "Indicador de metadata (1 byte): " << static_cast<int>(indicador) << "\n";
    output << "Capacidad disponible (4 bytes): " << capacidadDisponible << " bytes\n";

    output.close();

    std::cout << "Cabecera extraída en: " << archivoTxt << std::endl;
}
#endif
