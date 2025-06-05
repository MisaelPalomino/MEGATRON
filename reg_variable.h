#include "reg_fijo.h"


using namespace std;

struct Cabecera_Var {
    int cantidad;          // número de registros
    int siguiente_libre;   // posición para insertar el próximo registro

    Cabecera_Var() : cantidad(0), siguiente_libre(0) {}

    Cabecera_Var(const string& entrada) {
        if (entrada.size() >= 20) {
            string parte1 = entrada.substr(0, 10);
            string parte2 = entrada.substr(10, 10);

            parte1.erase(remove(parte1.begin(), parte1.end(), '_'), parte1.end());
            parte2.erase(remove(parte2.begin(), parte2.end(), '_'), parte2.end());

            try {
                cantidad = stoi(parte1);
            } catch (...) {
                cantidad = 0;
            }

            try {
                siguiente_libre = stoi(parte2);
            } catch (...) {
                siguiente_libre = 0;
            }
        } else {
            cantidad = 0;
            siguiente_libre = 0;
        }
    }

    string to_string() const {
        ostringstream oss;
        oss << setw(10) << setfill('_') << cantidad;
        oss << setw(10) << setfill('_') << siguiente_libre;
        return oss.str();
    }
};

void imprimir_cabecera_var(string cabecera) {
    cabecera.erase(remove(cabecera.begin(), cabecera.end(), '_'), cabecera.end());

    vector<string> partes;
    string parte;
    for (char c : cabecera) {
        if (c == '#') {
            partes.push_back(parte);
            parte.clear();
        } else {
            parte += c;
        }
    }
    if (!parte.empty()) partes.push_back(parte); 

    for (int i = 1; i < partes.size(); i += 2) {
        cout << partes[i] << " ";
    }
    cout << endl;
}

void Select_Todo_VAR(const string& cabecera,  vector<Bloque>& Relacion) {
    imprimir_cabecera_var(cabecera);

    int metadata_size = 23; // 10 bytes inicio + 1 separador + 10 bytes fin + 1 separador + 1 byte flag

    for (int i = 0; i < Relacion.size(); ++i) {
        for (int j = 0; j < Relacion[i].getCantidad_Sectores(); ++j) {
            const string& contenido = Relacion[i].getContenido(j);
            size_t pos = 0;

            while (pos + metadata_size <= contenido.size()) {
                string metadato = contenido.substr(pos, metadata_size);

                auto limpiar_numero = [](const string &s) -> int {
                    string limpio;
                    for (char c : s) {
                        if (isdigit(c)) limpio.push_back(c);
                        else if (c == '_') limpio.push_back('0');
                    }
                    if (limpio.empty()) return 0;
                    return stoi(limpio);
                };

                string ini_str = metadato.substr(0, 10);
                string fin_str = metadato.substr(11, 10);
                char flag_eliminado = metadato[22];

                if (flag_eliminado == '0') { // Registro válido, no eliminado
                    int ini = limpiar_numero(ini_str);
                    int fin = limpiar_numero(fin_str);
                    int tam_registro = fin - ini + 1;
                    size_t pos_registro = pos + metadata_size;

                    if (pos_registro + tam_registro <= contenido.size()) {
                        string registro = contenido.substr(pos_registro, tam_registro);

                        // Limpiar '_' y separar por '#'
                        string limpio;
                        for (char c : registro) {
                            if (c != '_') limpio += c;
                        }

                        vector<string> campos;
                        string campo_actual;
                        for (char c : limpio) {
                            if (c == '#') {
                                campos.push_back(campo_actual);
                                campo_actual.clear();
                            } else {
                                campo_actual += c;
                            }
                        }
                        if (!campo_actual.empty()) campos.push_back(campo_actual);

                        for (const string& campo : campos) {
                            cout << campo << " ";
                        }
                        cout << endl;
                    } else {
                        cout << "[ERROR] Registro fuera de rango en sector " << j << endl;
                    }
                }

                pos += metadata_size;

                // Saltar contenido del registro para posicionar al siguiente metadato
                // Para evitar errores, avanzamos hasta encontrar '_' que indica inicio de próxima metadata
                while (pos < contenido.size() && contenido[pos] != '_') pos++;
            }
        }
    }
}


string cabecera_CSV_var(fstream& file, string nombre) {
    string resultado = nombre + '#';
    char c;
    int col = 0;
    string tipo = "";
    int pos = 0;

    while (file.get(c)) {
        pos++;
        if (c == '\n') {
            file.seekg(pos);
            tipo = detectar_tipo(col, file);
            resultado += '#' + tipo;
            resultado+='\n';
            file.clear();
            break;
        } else if (c != ',' && c != '\n') {
            resultado += c;
        } else if (c == ',') {
            file.seekg(pos);
            tipo = detectar_tipo(col, file);
            file.clear();
            file.seekg(pos);

            resultado += '#' + tipo + '#';
            col++;
        }
    }
    return resultado;
}

bool verificarExistencia_var(vector<Bloque>& schemas, const string &tabla, string& cab, DiscoManager &manager, int &tam_sector) {
    const int TAM_LINEA = 251;
    for(int k=0; k<schemas.size(); k++){
        for (int i = 0; i < schemas[k].getCantidad_Sectores(); i++) {
            string contenido = schemas[k].getContenido(i);
            int total = contenido.size();
            for (int j = 0; j + TAM_LINEA <= total; j += TAM_LINEA) {
                string linea = contenido.substr(j, TAM_LINEA);
                size_t pos = linea.find('#');
                if (pos != string::npos) {
                    string nombre = linea.substr(0, pos);
                    string limpia = nombre;
                    limpia.erase(remove(limpia.begin(), limpia.end(), '_'), limpia.end());
                    if (limpia == tabla) {
                        string lineaSinGuiones = linea;
                        lineaSinGuiones.erase(remove(lineaSinGuiones.begin(), lineaSinGuiones.end(), '_'), lineaSinGuiones.end());
                        cab = lineaSinGuiones;
                        return true;
                    }
                }
            }
        }
    }
    fstream archivo(tabla + ".csv", ios::in);
    string cabecera = cabecera_CSV_var(archivo, tabla);
    stringstream ss;
    ss << left << setw(250) << setfill('_') << cabecera;
    ss << "\n";
    bool insertado=false;
    for(int i=0; i<schemas.size(); i++){
        insertado=insertarRegistro(schemas[i], ss.str(),251,tam_sector);
        if(insertado){
            manager.guardarEnDisco(schemas[i]);
            break;
        }
    }
    if(!insertado){
        Bloque b = manager.Ubicar_BloqueVacio("Schemas","fija");
        insertarRegistro(b,ss.str(),251,tam_sector);
        manager.guardarEnDisco(b);
    }
    cab = cabecera;
    return false;
}

int limpiar_numero(const string& s) {
    string limpio;
    for (char c : s) {
        if (isdigit(c)) limpio.push_back(c);
        else if (c == '_') limpio.push_back('0');
    }
    if (limpio.empty()) return 0;
    return stoi(limpio);
}

bool insertarRegistro_Var(Bloque &bloque, const string &registro, int tam_registro, int& tam_sector) {
    string linea_cabecera = bloque.get_cabecera(); 
    Cabecera_Var cabecera(linea_cabecera);

    const int metadata_size = 23;  // 10 + 1 + 10 + 1 + 1
    const int tam_total = metadata_size + tam_registro;

    // 1. Intentar reutilizar espacio eliminado
    for (int i = 0; i < bloque.getCantidad_Sectores(); ++i) {
        string &contenido = bloque.getContenidoRef(i);  
        size_t pos = 0;
        while (pos + metadata_size <= contenido.size()) {
            string metadato = contenido.substr(pos, metadata_size);
            if (metadato[22] == '1') { // Registro eliminado

                // Obtener inicio y fin del registro eliminado
                string ini_str = metadato.substr(0, 10);
                string fin_str = metadato.substr(11, 10);
                int ini = limpiar_numero(ini_str);
                int fin = limpiar_numero(fin_str);
                int espacio_registro = (fin - ini + 1) + metadata_size;

                if (espacio_registro >= tam_total) {
                    // Sobrescribir el contenido eliminado
                    stringstream ss;
                    ss << setw(10) << setfill('_') << ini << "|"
                       << setw(10) << setfill('_') << (ini + tam_registro - 1) << "|"
                       << "0" << registro;

                    contenido.replace(pos, ss.str().length(), ss.str());

                    cabecera.cantidad++;
                    cabecera.siguiente_libre=contenido.size();  // Actualiza con el final actual
                    bloque.set_cabecera(cabecera.to_string());
                    bloque.set_espacioDisponible(bloque.get_espacioDisponible() - tam_total);

                    cout << "[INFO] Registro reutilizado en: " << bloque.getSector(i) << ", pos " << pos << endl;
                    return true;
                }
            }

            // Mover al siguiente metadato (basado en tamaño fijo)
            pos += metadata_size;
            while (pos < contenido.size() && contenido[pos] != '_') pos++;
        }
    }

    // 2. Si no se encontró espacio reutilizable, insertar al final
    for (int i = 0; i < bloque.getCantidad_Sectores(); ++i) {
        string &contenido = bloque.getContenidoRef(i);
        int disponible = tam_sector - contenido.size();
        if (disponible >= tam_total) {
            int inicio = cabecera.siguiente_libre;
            int fin = inicio + tam_registro - 1;

            stringstream ss;
            ss << setw(10) << setfill('_') << inicio << "|"
               << setw(10) << setfill('_') << fin << "|"
               << "0" << registro;

            contenido += ss.str();

            cabecera.cantidad++;
            cabecera.siguiente_libre=contenido.size();  // Posición siguiente libre actualizada
            bloque.set_cabecera(cabecera.to_string());
            bloque.set_espacioDisponible(bloque.get_espacioDisponible() - tam_total);

            cout << "[INFO] Registro insertado nuevo en: " << bloque.getSector(i) << endl;
            return true;
        }
    }
    return false;
}



void contenido_total_var(fstream& file, string cabecera, DiscoManager &manager, 
                    int sectoresPorBloque, int& tam_sector, string nombre) {
    vector<string> tipos;
    istringstream ss(cabecera);
    string token;
    int index = 0;
    
    while (getline(ss, token, '#')) {
        if (index == 0) {
            // Nombre de relación ya lo tenemos
        } 
        else if (index % 2 == 0) {  
            tipos.push_back(token);
        } 
        index++;
    }


    vector<Bloque> bloques_relacion;
    manager.DevolverBloquesRelacion(nombre, bloques_relacion,"variable");
    salto_csv(file, '\n');
    char c;
    string campo_actual;
    int campo_index = 0;
    bool entre_comillas = false;
    string linea_formateada;
    int bloques_cont = 0;
    Bloque* bloque_actual = nullptr;
    size_t espacio_disponible = 0;

    if (!bloques_relacion.empty()) {
        bloque_actual = &bloques_relacion[0];
        espacio_disponible = bloque_actual->get_espacioDisponible();
        cout << "Usando bloque existente con espacio: " << espacio_disponible << endl;
    } else {
        Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre,"variable");
        bloques_relacion.push_back(nuevo_bloque);
        bloque_actual = &bloques_relacion.back();
        espacio_disponible = bloque_actual->get_espacioDisponible();
        cout << "Usando nuevo bloque con espacio: " << espacio_disponible << endl;
    }

    while (file.get(c)) {
        if (c == '"') {
            entre_comillas = !entre_comillas;
            continue;
        }

        if (c == ',' && !entre_comillas) {
            linea_formateada += campo_actual + "#";
            campo_actual.clear();
            campo_index++;
        } 
        else if (c == '\n') {
            linea_formateada += campo_actual + '\n';
            int tam_registro = linea_formateada.size();
            // Intentar insertar en bloques existentes primero
            bool insertado = false;
            for (auto& bloque : bloques_relacion) {
                if (insertarRegistro_Var(bloque, linea_formateada, tam_registro, tam_sector)) {
                    insertado = true;
                    bloque_actual = &bloque;
                    break;
                }
            }
            
            if (!insertado) {
                bloque_actual->imprimirBloque();
                Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre,"variable");
                if (nuevo_bloque.get_espacioDisponible() >= tam_registro) {
                    bloques_relacion.push_back(nuevo_bloque);
                    bloque_actual = &bloques_relacion.back();
                    insertarRegistro_Var(*bloque_actual, linea_formateada, tam_registro,tam_sector);
                    bloque_actual->imprimirBloque();
                    cout << "Nuevo bloque asignado para la relacion" << endl;
                } else {
                    cerr << "Error: No hay espacio suficiente en disco" << endl;
                    break;
                }
            }

            campo_actual.clear();
            campo_index = 0;
            entre_comillas = false;
            linea_formateada.clear();
        } 
        else {
            campo_actual += c;
        }
    }

    if (!campo_actual.empty()) {
        linea_formateada += campo_actual + '\n';
        int tam_registro = linea_formateada.size();
        bool insertado = false;
        for (auto& bloque : bloques_relacion) {
            if (insertarRegistro_Var(bloque, linea_formateada, tam_registro,tam_sector)) {
                insertado = true;
                break;
            }
        }

        if (!insertado) {
            Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre,"variable");
            if (nuevo_bloque.get_espacioDisponible() >= tam_registro) {
                bloques_relacion.push_back(nuevo_bloque);
                insertarRegistro_Var(bloques_relacion.back(), linea_formateada, tam_registro,tam_sector);
                cout << "Nuevo bloque asignado para último registro" << endl;
            } else {
                cerr << "Error: No hay espacio suficiente en disco para el último registro" << endl;
            }
        }
    }

    for (auto& bloque : bloques_relacion) {
        manager.guardarEnDisco(bloque);
        bloque.exportarTXTbloque(bloques_cont++, nombre);
    }

    cout << "Carga finalizada. Bloques utilizados: " << bloques_relacion.size() << endl;
}
void contenido_parcial_var(fstream& file, string cabecera, DiscoManager &manager, 
                          int sectoresPorBloque, int& tam_sector, string nombre, int N) {
    vector<string> tipos;
    istringstream ss(cabecera);
    string token;
    int index = 0;
    
    while (getline(ss, token, '#')) {
        if (index == 0) {
            // Nombre de relación
        } 
        else if (index % 2 == 0) {  
            tipos.push_back(token);
        } 
        index++;
    }

    vector<Bloque> bloques_relacion;
    manager.DevolverBloquesRelacion(nombre, bloques_relacion,"variable");
    salto_csv(file, '\n');
    char c;
    string campo_actual;
    int campo_index = 0;
    bool entre_comillas = false;
    string linea_formateada;
    int bloques_cont = 0;
    Bloque* bloque_actual = nullptr;
    size_t espacio_disponible = 0;

    if (!bloques_relacion.empty()) {
        bloque_actual = &bloques_relacion[0];
        espacio_disponible = bloque_actual->get_espacioDisponible();
        cout << "Usando bloque existente con espacio: " << espacio_disponible << endl;
    } else {
        Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre,"variable");
        bloques_relacion.push_back(nuevo_bloque);
        bloque_actual = &bloques_relacion.back();
        espacio_disponible = bloque_actual->get_espacioDisponible();
        cout << "Usando nuevo bloque con espacio: " << espacio_disponible << endl;
    }

    int registros_procesados = 0;
    bool fin_proceso = false;

    while (file.get(c) && !fin_proceso) {
        if (c == '"') {
            entre_comillas = !entre_comillas;
            continue;
        }

        if (c == ',' && !entre_comillas) {
            linea_formateada += campo_actual + "#";
            campo_actual.clear();
            campo_index++;
        } 
        else if (c == '\n') {
            linea_formateada += campo_actual + '\n';
            int tam_registro = linea_formateada.size();
            // Intentar insertar en bloques existentes primero
            bool insertado = false;
            for (auto& bloque : bloques_relacion) {
                if (insertarRegistro_Var(bloque, linea_formateada, tam_registro, tam_sector)) {
                    insertado = true;
                    bloque_actual = &bloque;
                    break;
                }
            }
            
            if (!insertado) {
                Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre,"variable");
                if (nuevo_bloque.get_espacioDisponible() >= tam_registro) {
                    bloques_relacion.push_back(nuevo_bloque);
                    bloque_actual = &bloques_relacion.back();
                    insertarRegistro_Var(*bloque_actual, linea_formateada, tam_registro,tam_sector);
                    cout << "Nuevo bloque asignado para la relacion" << endl;
                } else {
                    cerr << "Error: No hay espacio suficiente en disco" << endl;
                    break;
                }
            }

            registros_procesados++;
            if (registros_procesados >= N) {
                fin_proceso = true;
            }

            campo_actual.clear();
            campo_index = 0;
            entre_comillas = false;
            linea_formateada.clear();
        } 
        else {
            campo_actual += c;
        }
    }

    // Si cortamos en medio de un registro sin salto de línea:
    if (!fin_proceso && !campo_actual.empty()) {
        linea_formateada += campo_actual + '\n';
        int tam_registro = linea_formateada.size();
        bool insertado = false;
        for (auto& bloque : bloques_relacion) {
            if (insertarRegistro_Var(bloque, linea_formateada, tam_registro,tam_sector)) {
                insertado = true;
                break;
            }
        }

        if (!insertado) {
            Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre,"variable");
            if (nuevo_bloque.get_espacioDisponible() >= tam_registro) {
                bloques_relacion.push_back(nuevo_bloque);
                insertarRegistro_Var(bloques_relacion.back(), linea_formateada, tam_registro,tam_sector);
                cout << "Nuevo bloque asignado para último registro" << endl;
            } else {
                cerr << "Error: No hay espacio suficiente en disco para el último registro" << endl;
            }
        }
    }

    for (auto& bloque : bloques_relacion) {
        manager.guardarEnDisco(bloque);
        bloque.exportarTXTbloque(bloques_cont++, nombre);
    }

    cout << "Carga parcial finalizada. Registros procesados: " << registros_procesados << ", Bloques utilizados: " << bloques_relacion.size() << endl;
}

void Delete_where_VAR(string cabecera, vector<Bloque>& Relacion, string atributo, string valor, string comparador, int tam_sector, DiscoManager &manager) {
    vector<string> nombres, tipos;

    istringstream ss(cabecera);
    string token;
    int index = 0;

    while (getline(ss, token, '#')) {
        if (index == 0) {
        } else if (index % 2 == 1) {
            nombres.push_back(token);
        } else if (index % 2 == 0) {
            tipos.push_back(token);
        } 
        index++;
    }

    int campo_objetivo = -1;
    for (int i = 0; i < nombres.size(); ++i) {
        if (nombres[i] == atributo) {
            campo_objetivo = i;
            break;
        }
    }

    if (campo_objetivo == -1) {
        cout << "[ERROR] Atributo no encontrado: " << atributo << endl;
        return;
    }

    const int tam_metadata = 23;

    for (Bloque& bloque : Relacion) {
        string linea = bloque.get_cabecera();
        Cabecera_Var cab(linea);

        for (int sector = 0; sector < bloque.getCantidad_Sectores(); ++sector) {
            string& contenido = bloque.getContenidoRef(sector);

            size_t pos = 0;
            while (pos + tam_metadata <= contenido.size()) {
                string metadato = contenido.substr(pos, tam_metadata);
                if (metadato[22] == '1') {
                    pos += tam_metadata;
                    while (pos < contenido.size() && contenido[pos] != '_') pos++;
                    continue;
                }

                int inicio = limpiar_numero(metadato.substr(0, 10));
                int fin = limpiar_numero(metadato.substr(11, 10));
                int tam_registro = fin - inicio + 1;
                size_t pos_registro = pos + tam_metadata;

                if (pos_registro + tam_registro > contenido.size()) break;

                string registro = contenido.substr(pos_registro, tam_registro);
                string limpio;
                for (char c : registro) {
                    if (c != '_') limpio += c;
                }

                vector<string> campos;
                string campo;
                for (char c : limpio) {
                    if (c == '#') {
                        campos.push_back(campo);
                        campo.clear();
                    } else {
                        campo += c;
                    }
                }
                if (!campo.empty()) campos.push_back(campo);

                if (campo_objetivo >= campos.size()) {
                    pos += tam_metadata + tam_registro;
                    continue;
                }

                string campo_valor = campos[campo_objetivo];
                if (func_comparador(tipos[campo_objetivo], valor, campo_valor, comparador)) {
                    contenido[pos + 22] = '1';

                    cab.cantidad--;
                    bloque.set_cabecera(cab.to_string());

                    bloque.set_espacioDisponible(bloque.get_espacioDisponible() + tam_registro + tam_metadata);

                    cout << "[INFO] Registro eliminado en: " << bloque.getSector(sector) << endl;
                    manager.guardarEnDisco(bloque);
                    bloque.exportarTXTbloque(1,"Eliminado");
                }

                pos += tam_metadata;
                while (pos < contenido.size() && contenido[pos] != '_') pos++;
            }
        }
    }
}

void insertarRegistroDesdeTeclado_VAR(DiscoManager& manager, int sectoresPorBloque, int& tam_sector, const string& nombre, const string& cabecera) {
    vector<string> nombres;
    vector<string> tipos;
    istringstream ss(cabecera);
    string token;
    // Extraer nombres y tipos desde la cabecera
    int index = 0;

    while (getline(ss, token, '#')) {
        if (index == 0) {
        } else if (index % 2 == 1) {
            nombres.push_back(token);
        } else if (index % 2 == 0) {
            tipos.push_back(token);
        } 
        index++;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    // Leer valores por teclado
    string linea_formateada;
    cout << "=== Ingrese los datos del nuevo registro para la relación '" << nombre << "' ===" << endl;
    for (size_t i = 0; i < nombres.size(); ++i) {
        cout << nombres[i] << " (" << tipos[i] << "): ";
        string entrada;
        getline(cin, entrada);
        linea_formateada += entrada;
        if (i != nombres.size() - 1)
            linea_formateada += "#";
    }
    linea_formateada += "\n";

    int tam_registro = linea_formateada.size();

    // Buscar bloques ya existentes
    vector<Bloque> bloques_relacion;
    manager.DevolverBloquesRelacion(nombre, bloques_relacion, "variable");

    Bloque* bloque_actual = nullptr;
    int bloques_cont = 0;

    // Intentar insertar en bloques existentes
    for (auto& bloque : bloques_relacion) {
        if (insertarRegistro_Var(bloque, linea_formateada, tam_registro, tam_sector)) {
            bloque_actual = &bloque;
            cout << "Registro insertado en bloque existente." << endl;
            break;
        }
    }

    // Si no hay espacio, ubicar nuevo bloque
    if (!bloque_actual) {
        Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre, "variable");
        if (nuevo_bloque.get_espacioDisponible() >= tam_registro) {
            insertarRegistro_Var(nuevo_bloque, linea_formateada, tam_registro, tam_sector);
            manager.guardarEnDisco(nuevo_bloque);
            nuevo_bloque.exportarTXTbloque(bloques_cont++, nombre);
            cout << "Registro insertado en nuevo bloque." << endl;
        } else {
            cerr << "Error: No hay espacio suficiente para el nuevo registro." << endl;
        }
    } else {
        for (auto& bloque : bloques_relacion) {
            bloque.exportarTXTbloque(bloques_cont++, nombre);
            manager.guardarEnDisco(bloque);
        }
    }

    cout << "Registro insertado correctamente." << endl;
}

