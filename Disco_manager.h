#ifndef DISCOMANAGER_H
#define DISCOMANAGER_H

#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "Disco.h"
#include "Bloque.h"
#include "Metadata.h"
class DiscoManager {
private:
    Disco& disco;
    int sectoresPorBloque;
    int bloquesPorPista;
    string ruta_metadata;
    string ruta_schemas;
    size_t tam_sector;
    size_t tam_bloque;
    vector<Metadata> Diccionario;
public:
DiscoManager();
DiscoManager(Disco& d, int sec, size_t tam) : disco(d), sectoresPorBloque(sec), tam_sector(tam){
    bloquesPorPista=d.getNumSectores()/sec;
    tam_bloque=tam_sector*sectoresPorBloque;
    inicializarDiccionario();
    cargarDiccionario();
    inicializarSchemas();
}

void actualizarEspacioDisponible(const string& path_sector_0, int bytes_ocupados) {
    fstream archivo(path_sector_0, ios::in | ios::out);
    if (!archivo) {
        cerr << "No se pudo abrir el archivo para actualizar espacio: " << path_sector_0 << endl;
        return;
    }
    string s;
    archivo.seekg(0,ios::beg);
    char c;
    for(int i=0; i<12; i++){
        archivo.get(c);
        if(c!='_' && c!='\n'){
            s+=c;
        }
    }

    if (s.empty()) {
        cerr << "Error: No se encontró valor de espacio disponible." << endl;
        archivo.close();
        return;
    }

    int espacio_disponible;
    espacio_disponible = stoi(s);
    espacio_disponible -= bytes_ocupados;


    archivo.seekp(0, ios::beg);
    archivo << setw(12) << setfill('_') << espacio_disponible;
    
    archivo.close();
}

void avanzarPosicion(int &plato, int &superficie, int &sector, int &pista, Disco& disco) {
    superficie++;
    if (superficie >= disco.getNumSuperficies()) {
        superficie = 0;
        plato++;
        if (plato >= disco.getNumPlatos()) {
            plato = 0;
            sector++;
            if (sector >= disco.getNumSectores()) {
                sector = 0;
                pista++;
                if (pista >= disco.getNumPistas()) {
                    throw runtime_error("Se superó la cantidad total de posiciones.");
                }
            }
        }
    }
}

void inicializarDiccionario() {
    int cantidad_bloques = disco.get_cantidad_bloques();
    int espacio_disponible = tam_sector - 4;
    int registros_por_sector = espacio_disponible / 51;
    int bloques_por_sector = registros_por_sector;
    int bloques_por_metadata = bloques_por_sector * sectoresPorBloque;

    int cantidad_bloques_metadata = cantidad_bloques / bloques_por_metadata;
    if (cantidad_bloques % bloques_por_metadata != 0)
        cantidad_bloques_metadata++;

    int registros_en_sector = 0;
    int sector_actual_idx = 0;
    int contador = 0;

    vector<string> sectores_metadata;

    for (int pista = 0; pista < disco.getNumPistas(); ++pista) {
        for (int sector = 0; sector < disco.getNumSectores(); ++sector) {
            for (int plato = 0; plato < disco.getNumPlatos(); ++plato) {
                for (int superficie = 0; superficie < disco.getNumSuperficies(); ++superficie) {
                    if (contador >= cantidad_bloques_metadata * sectoresPorBloque)
                        goto sectores_creados;

                    string direccion = disco.getPlato(plato)
                                           ->getSuperficie(superficie)
                                           ->getPista(pista)
                                           ->getSector(sector)
                                           ->getPath();

                    ofstream archivo(direccion, ios::trunc | ios::out);
                    if (!archivo) {
                        cerr << "Error al crear sector de diccionario: " << direccion << endl;
                        return;
                    }

                    if (contador == 0) {
                        archivo << setw(12) << setfill('_') << espacio_disponible << '\n'; 
                        ruta_metadata = direccion;
                    }else{
                        archivo << setw(12) << setfill('_') << espacio_disponible << '\n'; 
                    }

                    archivo.close();
                    sectores_metadata.push_back(direccion);
                    contador++;
                }
            }
        }
    }

sectores_creados:

    int superficie_actual = 0;
    int plato_actual = 0;
    int sector_actual = 0;
    int pista_actual = 0;

    int sectores_usados = 0;
    int total_sectores_disco = disco.getNumPlatos() * disco.getNumSuperficies() * disco.getNumPistas() * disco.getNumSectores();

    for (int i = 0; i < cantidad_bloques; ++i) {
        if (sector_actual_idx >= sectores_metadata.size()) {
            cerr << "Error: No hay suficientes sectores de metadata." << endl;
            return;
        }

        fstream archivo(sectores_metadata[sector_actual_idx],ios::in | ios::out);
        archivo.seekp(0,ios::end);
        if (!archivo) {
            cerr << "Error al abrir sector de metadata." << endl;
            return;
        }
        if (registros_en_sector == 0) {
            espacio_disponible = tam_sector - 4;  // solo al empezar nuevo sector
        }
        string nombre = (i < cantidad_bloques_metadata) ? "Metadata" : "NULL";
        archivo << setw(16) << setfill('_') << left << nombre << '\n';
        archivo << setw(10) << setfill('_') << plato_actual;
        archivo << setw(10) << setfill('_') << superficie_actual;
        archivo << setw(10) << setfill('_') << pista_actual;
        archivo << setw(10) << setfill('_') << sector_actual << '\n';

        int platof = plato_actual;
        int superficief = superficie_actual;
        int pistaf = pista_actual;
        int sectorf = sector_actual;

        int sectores_en_este_bloque = std::min(sectoresPorBloque, total_sectores_disco - sectores_usados);

        try {
            for (int r = 1; r < sectores_en_este_bloque; ++r) {
                avanzarPosicion(platof, superficief, sectorf, pistaf, disco);
            }
        } catch (const runtime_error &e) {
            cerr << "Error durante cálculo de posición final: " << e.what() << endl;
            return;
        }

        archivo << setw(10) << setfill('_') << platof;
        archivo << setw(10) << setfill('_') << superficief;
        archivo << setw(10) << setfill('_') << pistaf;
        archivo << setw(10) << setfill('_') << sectorf << '\n';

        archivo.flush();
        int bytes_usados = 0;
        bytes_usados += 16 + 1; // nombre + '\n'
        bytes_usados += (4 * 4) + 1; // inicio: plato, superficie, pista, sector + '\n'
        bytes_usados += (4 * 4) + 1; // fin: plato, superficie, pista, sector + '\n'

        espacio_disponible -= bytes_usados;
        if (espacio_disponible < 0) espacio_disponible = 0;  // prevenir valores negativos

        // Actualizar cabecera con nuevo espacio disponible
        archivo.seekp(0, ios::beg);
        archivo << setw(12) << setfill('_') << espacio_disponible << '\n';

        archivo.close();

        try {
            // Avanzamos solo si quedan más sectores por usar después de este bloque
            if (sectores_usados + sectores_en_este_bloque < total_sectores_disco) {
                for (int r = 0; r < sectores_en_este_bloque; ++r) {
                    avanzarPosicion(plato_actual, superficie_actual, sector_actual, pista_actual, disco);
                }
            }
            // Si ya llegamos al final, no avanzamos más
        } catch (const runtime_error &e) {
            cerr << "Error al avanzar a siguiente bloque: " << e.what() << endl;
            return;
        }

        sectores_usados += sectores_en_este_bloque;

        registros_en_sector++;
        if (registros_en_sector >= registros_por_sector) {
            sector_actual_idx++;
            registros_en_sector = 0;
        }
    }

    cout << "Bloques inicializados correctamente." << endl;
}

void cargarDiccionario() {
    Diccionario.clear();
    ifstream archivo(ruta_metadata);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo: " << ruta_metadata << endl;
        return;
    }

    string linea;
    getline(archivo, linea); // Línea de espacio disponible, ignorar

    vector<string> lineas;
    while (getline(archivo, linea)) {
        if (!linea.empty())
            lineas.push_back(linea);
    }

    for (size_t i = 0; i < lineas.size(); ++i) {
        if (lineas[i].substr(0, 8) == "Metadata") {
            if (i + 2 >= lineas.size()) {
                cerr << "Coordenadas incompletas para Metadata en línea " << i << endl;
                continue;
            }

            // --- Coordenadas de inicio ---
            string inicio = lineas[i + 1];
            int pl_ini = stoi(inicio.substr(0, 10));
            int sup_ini = stoi(inicio.substr(10, 10));
            int pis_ini = stoi(inicio.substr(20, 10));
            int sec_ini = stoi(inicio.substr(30, 10));

            // --- Coordenadas de fin ---
            string fin = lineas[i + 2];
            int pl_fin = stoi(fin.substr(0, 10));
            int sup_fin = stoi(fin.substr(10, 10));
            int pis_fin = stoi(fin.substr(20, 10));
            int sec_fin = stoi(fin.substr(30, 10));

            Metadata nuevo;

            // Inicializar contadores con la coordenada inicial
            int sup = sup_ini, pl = pl_ini, pis = pis_ini, sec = sec_ini;

            while (true) {
                string path = disco.getPlato(pl)->getSuperficie(sup)->getPista(pis)->getSector(sec)->getPath();
                nuevo.agregarSector(path);
                // Condición de parada
                if (sup == sup_fin && pl == pl_fin && sec == sec_fin && pis == pis_fin) {
                    break;
                }

                // Avance según el orden: superficie → plato → sector → pista
                ++sup;
                if (sup >= disco.getNumSuperficies()) {
                    sup = 0;
                    ++pl;
                    if (pl >= disco.getNumPlatos()) {
                        pl = 0;
                        ++sec;
                        if (sec >= disco.getNumSectores()) {
                            sec = 0;
                            ++pis;
                        }
                    }
                }
            }


            Diccionario.push_back(nuevo);
            i += 2; // Saltamos coordenadas ya procesadas
        }
    }

    archivo.close();
}
void guardarDiccionario(){
    for(int i=0; i<Diccionario.size(); i++){
        for(int j=0; j<Diccionario[i].cantidad_sectores(); j++){
            fstream archivo(Diccionario[i].getReferencia(j), ios::out | ios::trunc);
            archivo<< std::left<<setw(12)<<setfill('_')<<to_string(Diccionario[i].get_tam_disponible(j))<<'\n';
            archivo<<Diccionario[i].getContenido_sector_guardar(j);
            archivo.close();
        }
    }
}
void inicializarSchemas() {
    Bloque b=Ubicar_BloqueVacio("Schemas","fija");
}

void DevolverBloquesRelacion(const string& nombreBuscado, vector<Bloque>& Bloques_relacion, string opcion) {
    int nuevaInicio[4];
    int nuevaFin[4];
    bool encontrado = false;

    for (int i = 0; i < Diccionario.size(); i++) {
        int index = 0;
        for (int j = 0; j < Diccionario[i].cantidad_sectores(); j++) {
            int cantidadReg = Diccionario[i].get_cant_reg_Sector(j);
            for (int k = 0; k < cantidadReg; k++, index++) {
                if (Diccionario[i].getNombre(index) == nombreBuscado) {
                    Coord cIni = Diccionario[i].getCordsIni(index);
                    Coord cFin = Diccionario[i].getCordsFin(index);

                    nuevaInicio[0] = cIni.plato;
                    nuevaInicio[1] = cIni.superficie;
                    nuevaInicio[2] = cIni.pista;
                    nuevaInicio[3] = cIni.sector;
                    
                    nuevaFin[0] = cFin.plato;
                    nuevaFin[1] = cFin.superficie;
                    nuevaFin[2] = cFin.pista;
                    nuevaFin[3] = cFin.sector;

                    Bloque bloque;
                    construirBloque(bloque, nuevaInicio, nuevaFin,opcion);
                    Bloques_relacion.push_back(bloque);

                    encontrado = true;
                }
            }
        }
    }
}


Bloque Ubicar_BloqueVacio(const string& nombreRelacion, string opcion) {
    Bloque bloque;
    int nuevaInicio[4] = {-1, -1, -1, -1}; 
    int nuevaFin[4] = {-1, -1, -1, -1};
    bool encontrado = false;

    for (int i = 0; i < Diccionario.size() && !encontrado; i++) {
        int index = 0;
        for (int j = 0; j < Diccionario[i].cantidad_sectores() && !encontrado; j++) {
            int cantidadReg = Diccionario[i].get_cant_reg_Sector(j);
            for (int k = 0; k < cantidadReg && !encontrado; k++) {
                if (Diccionario[i].getNombre(index) == "NULL") {
                    Coord cIni = Diccionario[i].getCordsIni(index);
                    Coord cFin = Diccionario[i].getCordsFin(index);

                    nuevaInicio[0] = cIni.plato;
                    nuevaInicio[1] = cIni.superficie;
                    nuevaInicio[2] = cIni.pista;
                    nuevaInicio[3] = cIni.sector;
                
                    nuevaFin[0] = cFin.plato;
                    nuevaFin[1] = cFin.superficie;
                    nuevaFin[2] = cFin.pista;
                    nuevaFin[3] = cFin.sector;

                    
                    Diccionario[i].setNombre(index, nombreRelacion);
                    encontrado = true;
                }
                index++;
            }
        }
    }

    if (!encontrado) {
        throw runtime_error("No se encontró un bloque vacío en el diccionario.");
    }
    
    construirBloque(bloque, nuevaInicio, nuevaFin,opcion);
    guardarDiccionario();
    return bloque;
}

void construirBloque(Bloque& bloque, const int* inicio, const int* fin, string opcion) {
    int platos = disco.getNumPlatos();
    int superficies = disco.getPlato(0)->getNumSuperficies();
    int pistas = disco.getPlato(0)->getSuperficie(0)->getNumPistas();
    int sectores = disco.getPlato(0)->getSuperficie(0)->getPista(0)->getNumSectores();

    int pos[4] = {inicio[0], inicio[1], inicio[2], inicio[3]};
    int count = 0;

    while (true) {
        int plato = pos[0];
        int sup = pos[1];
        int pista = pos[2];
        int sector = pos[3];

        if (plato >= platos || sup >= superficies || pista >= pistas || sector >= sectores) {
            throw runtime_error("Error: Posición fuera de rango en construirBloque.");
        }

        Sector* s = disco.getPlato(plato)->getSuperficie(sup)->getPista(pista)->getSector(sector);

        bool esCabecera = (count == 0); 
        inicializarSector(s->getPath(), esCabecera,opcion);
        bloque.agregarSector(s->getPath());
        count++;

        if (plato == fin[0] && sup == fin[1] && pista == fin[2] && sector == fin[3]) {
            break;
        }

        // Avance: superficie → plato → sector → pista
        pos[1]++;
        if (pos[1] >= superficies) {
            pos[1] = 0;
            pos[0]++;
            if (pos[0] >= platos) {
                pos[0] = 0;
                pos[3]++;
                if (pos[3] >= sectores) {
                    pos[3] = 0;
                    pos[2]++;
                    if (pos[2] >= pistas) {
                        throw runtime_error("Error: Se acabaron las pistas antes de completar el bloque.");
                    }
                }
            }
        }

        if (count >= sectoresPorBloque) break;
    }
}


void inicializarSector(const string& s, bool esCabecera, string opcion) {
    ifstream verificador(s);
    if (verificador.peek() != ifstream::traits_type::eof()) {
        verificador.close();
        return; 
    }
    verificador.close();

    fstream archivo(s, ios::out);
    if (!archivo.is_open()) {
        cerr << "Error al inicializar el sector: " << s << endl;
        return;
    }

    if (esCabecera) {
        if(opcion=="fija"){
            int espacio_disponible = tam_sector*sectoresPorBloque - 4;
            archivo << setw(12) << setfill('_') << espacio_disponible << '\n';
            archivo << setw(10) << setfill('_') << -1 << '\n';
        }else {
            int espacio_disponible = tam_sector*sectoresPorBloque - 4;
            archivo << setw(12) << setfill('_') << espacio_disponible << '\n';
            archivo << setw(10) << setfill('_') << 0 ;
            archivo << setw(10) << setfill('_') << 0 << '\n';
        }
    } else {
        archivo << ""; //posible causa de errores;
    }

    archivo.close();
}

void guardarEnDisco(Bloque &b){
    int usado=tam_bloque-b.get_espacioDisponible();
    disco.actualizar_tamano(disco.Get_Capacidad_Disco()-usado);
    for(int i=0; i<b.getCantidad_Sectores(); i++){
        fstream archivo(b.getSector(i),ios::out |ios::trunc);
        if(i==0){
            archivo << setw(12) << setfill('_') << b.get_espacioDisponible();
            archivo.put('\n');
            archivo << b.get_cabecera();
            archivo.put('\n');
        }
        archivo<<b.getContenido(i);
    }
}
void mostrar_BloquesOcupados(){
    for (int i = 0; i < Diccionario.size(); i++) {
        int index = 0;
        for (int j = 0; j < Diccionario[i].cantidad_sectores(); j++) {
            int cantidadReg = Diccionario[i].get_cant_reg_Sector(j);
            for (int k = 0; k < cantidadReg; k++, index++) {
                if (Diccionario[i].getNombre(index) != "NULL") {
                    cout<<"Bloque Ocupado"<<endl;
                    cout<<Diccionario[i].getNombre(index)<<endl;
                    cout<<"Cordenas Sectores:"<<endl;
                    Coord cIni = Diccionario[i].getCordsIni(index);
                    Coord cFin = Diccionario[i].getCordsFin(index);
                    cout<<cIni.plato<<","<<cIni.superficie<<","<<cIni.pista<<","<<cIni.sector<<endl;
                    cout<<cFin.plato<<","<<cFin.superficie<<","<<cFin.pista<<","<<cFin.sector<<endl;
                    fstream archivo("Disco/Plato_"+to_string(cIni.plato) +"/Superficie_"+to_string(cIni.superficie) +"/Pista_"+to_string(cIni.pista) +"/Sector_"+to_string(cIni.sector) +".txt",ios::in);
                    string linea;
                    getline(archivo,linea);
                    cout<<"Capacidad disponible: "<<endl;
                    cout<<linea;
                    archivo.close();
                    cout<<endl<<endl;
                }
            }
        }
    }
}

void mostrar_BloqueVacio(){
    for (int i = 0; i < Diccionario.size(); i++) {
        int index = 0;
        for (int j = 0; j < Diccionario[i].cantidad_sectores(); j++) {
            int cantidadReg = Diccionario[i].get_cant_reg_Sector(j);
            for (int k = 0; k < cantidadReg; k++, index++) {
                if (Diccionario[i].getNombre(index) == "NULL") {
                    cout<<"Bloque Libre"<<endl;
                    cout<<Diccionario[i].getNombre(index)<<endl;
                    cout<<"Cordenas Sectores:"<<endl;
                    Coord cIni = Diccionario[i].getCordsIni(index);
                    Coord cFin = Diccionario[i].getCordsFin(index);
                    cout<<cIni.plato<<","<<cIni.superficie<<","<<cIni.pista<<","<<cIni.sector<<endl;
                    cout<<cFin.plato<<","<<cFin.superficie<<","<<cFin.pista<<","<<cFin.sector<<endl;
                    cout<<"Capacidad disponible: "<<endl;
                    cout<<sectoresPorBloque*tam_sector<<endl;
                    cout<<endl;
                }
            }
        }
    }
}
    void mostrar_SectoresOcupados() {
        cout << "Sectores Ocupados: " << endl;
        int cont=0;
        for (int i = 0; i < Diccionario.size(); i++) {
            int index = 0;
            for (int j = 0; j < Diccionario[i].cantidad_sectores(); j++) {
                int cantidadReg = Diccionario[i].get_cant_reg_Sector(j);
                for (int k = 0; k < cantidadReg; k++, index++) {
                    if (Diccionario[i].getNombre(index) != "NULL") {
                        cont++;
                        Coord cIni = Diccionario[i].getCordsIni(index);
                        Coord cFin = Diccionario[i].getCordsFin(index);
                        int plato = cIni.plato;
                        int superficie = cIni.superficie;
                        int sector = cIni.sector;
                        int pista = cIni.pista;

                        while (true) {
                            cout << "(" << plato << "," << superficie << "," << pista << "," << sector << ")" << endl;
                            //verificar que ya se llego al final
                            if (plato == cFin.plato &&
                                superficie == cFin.superficie &&
                                sector == cFin.sector &&
                                pista == cFin.pista) {
                                break;
                            }

                            avanzarPosicion(plato, superficie, sector, pista, disco);
                        }

                        cout << endl;
                    }
                }
            }
        }
        cout<<"Cantidad: "<<cont<<endl;
    }

};



#endif
