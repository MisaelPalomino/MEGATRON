#include <iostream>
#include "Disco_manager.h"

void Select_where(string cabecera, vector<Bloque>& Relacion, string atributo, string valor, string comparador) {
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

    vector<string> nombres, tipos;
    vector<int> tamanios;
    for (int i = 1; i < partes.size(); i += 3) {
        nombres.push_back(partes[i]);
        tipos.push_back(partes[i + 1]);
        tamanios.push_back(stoi(partes[i + 2]));
    }

    int index = -1;
    for (int i = 0; i < nombres.size(); ++i) {
        if (nombres[i] == atributo) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        cout << "[ERROR] Atributo no encontrado: " << atributo << endl;
        return;
    }

    for (const string& name : nombres) {
        cout << name << " ";
    }
    cout << endl;

    for (int i = 0; i < Relacion.size(); ++i) {
        for (int j = 0; j < Relacion[i].getCantidad_Sectores(); ++j) {
            string contenido = Relacion[i].getContenido(j);
            int tam_registro = contenido.find('\n') + 1;
            if (tam_registro == 0) continue;

            for (int k = 0; k + tam_registro <= contenido.size(); k += tam_registro) {
                string registro = contenido.substr(k, tam_registro);

                if (!registro.empty() && registro[0] == '*') continue;

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

                if (index < campos.size() && func_comparador(tipos[index], valor, campos[index], comparador)) {
                    for (const string& campo_imp : campos) {
                        cout << campo_imp << " ";
                    }
                }
            }
        }
    }
}

void imprimir_cabecera(string cabecera) {
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

    for (int i = 1; i < partes.size(); i += 3) {
        cout << partes[i] << " ";
    }
    cout << endl;
}

void Select_Todo(string cabecera, vector<Bloque>& Relacion) {
    imprimir_cabecera(cabecera);

    for (int i = 0; i < Relacion.size(); ++i) {
        for (int j = 0; j < Relacion[i].getCantidad_Sectores(); ++j) {
            string contenido = Relacion[i].getContenido(j);
            int tam_registro = contenido.find('\n') + 1;
            if (tam_registro == 0) continue;

            for (int k = 0; k + tam_registro <= contenido.size(); k += tam_registro) {
                string registro = contenido.substr(k, tam_registro);

                if (registro.empty() || registro[0] == '*') continue;

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
            }
        }
    }
}


void salto_csv(fstream& file, char obviar) {
    char c;
    bool entre_comillas = false;
    while (file.get(c)) {
        if (c == '"') {
            entre_comillas = !entre_comillas;
        } else if (c == obviar && !entre_comillas) {
            break; 
        }
    }
}

struct Cabecera_fija {
    int inicio_lista_eliminada;

    Cabecera_fija() : inicio_lista_eliminada(-1) {}

    Cabecera_fija(const string& entrada) {
        string limpio;
        for (char c : entrada) {
            if (isdigit(c) || c == '-') {
                limpio += c;
            }
        }
        try {
            inicio_lista_eliminada = stoi(limpio);
        } catch (...) {
            inicio_lista_eliminada = -1; 
        }
    }
};


string extraerContenido_CSV(fstream& file){
    string p;
    char c;
    bool entre_comillas = false;
    while (file.get(c)) {
        if (c == '"') {
            entre_comillas = !entre_comillas;
        } else if (c == ',' && !entre_comillas) {
            break; 
        } else if (c == '\n') {
            break;
        } else {
            p+=c;
        }
    }
    return p;
}

int max_str(fstream& file, int col){
    file.seekg(0,ios::beg);
    salto_csv(file,'\n');
    int n=0;
    int max=0;
    char c;
    bool entre_comillas = false;
    while(file){
        for(int i=0; i<col; i++){
            salto_csv(file,',');
        }
        while(file.get(c)){
            if(c=='"'){
                entre_comillas = !entre_comillas;
            } else if (c==',' && !entre_comillas) {
                break;
            }else {
                n++;
            }
        }
        salto_csv(file,'\n');
        if(n>=max){
            max=n;
        }
        n=0;
    }
    file.clear();
    return max;
}

string detectar_tipo(int col,fstream& entrada){
    salto_csv(entrada,'\n');
    string valor;
    while(entrada){
        for(int i=0; i<col; i++){
            salto_csv(entrada,',');
        }
        valor=extraerContenido_CSV(entrada);
        if(valor==""){
            salto_csv(entrada,'\n');
        }else{
            break;
        }
    }
    return detectarTipo(valor);
}

string cabecera_CSV(fstream& file, string nombre) {
    string resultado = nombre + '#';
    char c;
    int col = 0;
    string tipo = "";
    int tam;
    int pos = 0;

    while (file.get(c)) {
        pos++;
        if (c == '\n') {
            file.seekg(pos);
            tipo = detectar_tipo(col, file);
            file.clear();

            resultado += '#' + tipo;
            if (tipo == "str") {
                tam = max_str(file, col);
                resultado += '#' + to_string(tam) + '\n';
                file.seekg(pos);
            } else if (tipo == "int") {
                resultado += "#10\n";
            } else {
                resultado += "#8\n";
            }
            break;
        } else if (c != ',' && c != '\n') {
            resultado += c;
        } else if (c == ',') {
            file.seekg(pos);
            tipo = detectar_tipo(col, file);
            file.clear();
            file.seekg(pos);

            resultado += '#' + tipo;
            if (tipo == "str") {
                tam = max_str(file, col);
                resultado += '#' + to_string(tam) + '#';
                file.seekg(pos);
            } else if (tipo == "int") {
                resultado += "#10#";
            } else {
                resultado += "#8#";
            }
            col++;
        }
    }
    return resultado;
}
string formatear_campo(const string& valor, const string& tipo, int tam) {
    string resultado = valor;
    
    if (!resultado.empty() && resultado.front() == '"' && resultado.back() == '"') {
        resultado = resultado.substr(1, resultado.size() - 2);
    }
    
    if (tipo == "str") {
        if (resultado.size() > tam) {
            resultado = resultado.substr(0, tam);
        } else {
            resultado += string(tam - resultado.size(), '_');
        }
    } 
    else {  
        if (resultado.empty()) {
            resultado = string(tam, '_');
        } else {
            bool es_numero = !valor.empty() && (isdigit(valor[0]) || (valor[0] == '-' && valor.size() > 1));
            
            if (!es_numero) {
                resultado = string(tam, '_');
            } else {
                if (resultado.size() < tam) {
                    resultado = string(tam - resultado.size(), '_') + resultado;
                }
            }
        }
    }
    
    return resultado;
}
int calcular_tamano_registro(const vector<int>& tamanos) {
    int total = 0;

    for (size_t i = 0; i < tamanos.size(); ++i) {
        total += tamanos[i];       
        if (i < tamanos.size() - 1)
            total += 1;            
    }

    total += 1; 

    return total;
}


Disco crearDisco() {
    int platos, pistas, sectores, tam_sector, sectoresPorBloque;
    cout << "\n===== CREACION DEL DISCO =====\n";
    cout << "Numero de platos: "; cin >> platos;
    cout << "Numero de pistas por superficie: "; cin >> pistas;
    cout << "Numero de sectores por pista: "; cin >> sectores;
    cout << "Tamano del sector (Bytes): "; cin >> tam_sector;
    cout << "Numero de sectores por bloque: "; cin >> sectoresPorBloque;

    Disco d(platos, 2, pistas, sectores, tam_sector, sectoresPorBloque);
    cout << " Disco creado con éxito.\n";
    d.capacidad_del_disco();
    cout<<"Capacidad de Bloque: "<<tam_sector*sectoresPorBloque<<'\n';
    cout<<"Cantidad de Bloques por pista: "<<sectores/sectoresPorBloque<<'\n';
    cout<<"Cantidad de Bloques por plato: "<<sectores*2*pistas/sectoresPorBloque<<'\n';
    d.mostrar_arbol();
    return d;
}

bool insertarRegistro(Bloque &bloque, const string &registro, int tam, int& tam_sector) {
    string linea = bloque.get_cabecera();
    Cabecera_fija c(linea);

    if (c.inicio_lista_eliminada != -1) {
        // Hay un espacio libre en la lista de eliminados
        int tam_disponible = bloque.get_espacioDisponible();
        int pos = c.inicio_lista_eliminada;
        string contenido = bloque.getContenido_en_posicion(pos, registro.size());

        int siguiente;
        try {
            string campo = contenido.substr(1, 10); // Extraer 10 caracteres
            string limpio;
            for (char c : campo) {
                if (isdigit(c)) limpio += c;
                else if (c == '-') limpio += c;
            }
            siguiente = limpio.empty() ? -1 : stoi(limpio);
        } catch (...) {
            siguiente = -1;
        }

        stringstream ss;
        ss << setw(10) << setfill('_') << siguiente;
        int basura;
        bloque.set_espacioDisponible(tam_disponible - tam);
        bloque.setContenido_en_posicion(pos, registro, registro.size(), basura);
        bloque.set_cabecera(ss.str());
        cout << "[INFO] Registro insertado en posición reutilizada: " << pos << endl;
    } else {
        int tam_disponible = bloque.get_espacioDisponible();

        if (tam <= 0 || tam > tam_disponible) {
            bloque.set_espacioDisponible(0);  
            return false;
        }

        int cantidad_sectores = bloque.getCantidad_Sectores();
        int registros_por_sector = tam_sector / tam;
        if (registros_por_sector == 0) {
            bloque.set_espacioDisponible(0);
            return false;
        }

        int registros_totales = cantidad_sectores * registros_por_sector;
        int registros_ocupados = (cantidad_sectores * tam_sector - tam_disponible) / tam;
        if (registros_ocupados >= registros_totales) {
            bloque.set_espacioDisponible(0);
            return false;
        }

        int sector_objetivo = registros_ocupados / registros_por_sector;
        if (sector_objetivo >= cantidad_sectores) {
            bloque.set_espacioDisponible(0);
            return false;
        }

        bloque.addContenido(sector_objetivo, registro);
        bloque.set_espacioDisponible(tam_disponible - tam);
        cout << "[INFO] Registro insertado en sector: " << bloque.getSector(sector_objetivo) << endl;
    }

    return true;
}




void contenido_total(fstream& file, string cabecera, DiscoManager &manager, 
                    int sectoresPorBloque, int& tam_sector, string nombre) {
    // 1. Parsear la cabecera para obtener estructura
    vector<int> tamanos;
    vector<string> tipos;
    istringstream ss(cabecera);
    string token;
    int index = 0;
    
    while (getline(ss, token, '#')) {
        if (index == 0) {
            // Nombre de relación ya lo tenemos
        } 
        else if (index % 3 == 0) {  
            tamanos.push_back(stoi(token));
        } 
        else if (index % 3 == 2) {  
            tipos.push_back(token);
        }
        index++;
    }

    int tam_registro = calcular_tamano_registro(tamanos);
    cout << "Tamaño de registro: " << tam_registro << endl;

    vector<Bloque> bloques_relacion;
    manager.DevolverBloquesRelacion(nombre, bloques_relacion,"fija");
    
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
        Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre,"fija");
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
            linea_formateada += formatear_campo(campo_actual, tipos[campo_index], tamanos[campo_index]) + "#";
            campo_actual.clear();
            campo_index++;
        } 
        else if (c == '\n') {
            linea_formateada += formatear_campo(campo_actual, tipos[campo_index], tamanos[campo_index]) + '\n';
            
            // Intentar insertar en bloques existentes primero
            bool insertado = false;
            for (auto& bloque : bloques_relacion) {
                if (insertarRegistro(bloque, linea_formateada, tam_registro, tam_sector)) {
                    insertado = true;
                    bloque_actual = &bloque;
                    break;
                }
            }
            
            if (!insertado) {
                Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre,"fija");
                if (nuevo_bloque.get_espacioDisponible() >= tam_registro) {
                    bloques_relacion.push_back(nuevo_bloque);
                    bloque_actual = &bloques_relacion.back();
                    insertarRegistro(*bloque_actual, linea_formateada, tam_registro,tam_sector);
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
        linea_formateada += formatear_campo(campo_actual, tipos[campo_index], tamanos[campo_index]) + '\n';
        
        bool insertado = false;
        for (auto& bloque : bloques_relacion) {
            if (insertarRegistro(bloque, linea_formateada, tam_registro,tam_sector)) {
                insertado = true;
                break;
            }
        }

        if (!insertado) {
            Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre,"fija");
            if (nuevo_bloque.get_espacioDisponible() >= tam_registro) {
                bloques_relacion.push_back(nuevo_bloque);
                insertarRegistro(bloques_relacion.back(), linea_formateada, tam_registro,tam_sector);
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

void contenido_parcial(fstream& file, string cabecera, DiscoManager &manager, 
                    int sectoresPorBloque, int& tam_sector, string nombre, int N) {
    // 1. Parsear la cabecera para obtener estructura
    vector<int> tamanos;
    vector<string> tipos;
    istringstream ss(cabecera);
    string token;
    int index = 0;
    
    while (getline(ss, token, '#')) {
        if (index == 0) {
            // Nombre de relación ya lo tenemos
        } 
        else if (index % 3 == 0) {  
            tamanos.push_back(stoi(token));
        } 
        else if (index % 3 == 2) {  
            tipos.push_back(token);
        }
        index++;
    }

    int tam_registro = calcular_tamano_registro(tamanos);
    cout << "Tamaño de registro: " << tam_registro << endl;

    vector<Bloque> bloques_relacion;
    manager.DevolverBloquesRelacion(nombre, bloques_relacion,"fija");
    
    salto_csv(file, '\n'); // Saltar cabecera CSV

    char c;
    string campo_actual;
    int campo_index = 0;
    bool entre_comillas = false;
    string linea_formateada;
    int bloques_cont = 0;
    int registros_insertados = 0;

    Bloque* bloque_actual = nullptr;
    size_t espacio_disponible = 0;

    if (!bloques_relacion.empty()) {
        bloque_actual = &bloques_relacion[0];
        espacio_disponible = bloque_actual->get_espacioDisponible();
        cout << "Usando bloque existente con espacio: " << espacio_disponible << endl;
    } else {
        Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre,"fija");
        bloques_relacion.push_back(nuevo_bloque);
        bloque_actual = &bloques_relacion.back();
        espacio_disponible = bloque_actual->get_espacioDisponible();
        cout << "Usando nuevo bloque con espacio: " << espacio_disponible << endl;
    }

    while (file.get(c) && registros_insertados < N) {
        if (c == '"') {
            entre_comillas = !entre_comillas;
            continue;
        }

        if (c == ',' && !entre_comillas) {
            linea_formateada += formatear_campo(campo_actual, tipos[campo_index], tamanos[campo_index]) + "#";
            campo_actual.clear();
            campo_index++;
        } 
        else if (c == '\n') {
            linea_formateada += formatear_campo(campo_actual, tipos[campo_index], tamanos[campo_index]) + '\n';
            
            // Intentar insertar
            bool insertado = false;
            for (auto& bloque : bloques_relacion) {
                if (insertarRegistro(bloque, linea_formateada, tam_registro, tam_sector)) {
                    insertado = true;
                    bloque_actual = &bloque;
                    registros_insertados++;
                    break;
                }
            }

            if (!insertado && registros_insertados < N) {
                Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre,"fija");
                if (nuevo_bloque.get_espacioDisponible() >= tam_registro) {
                    bloques_relacion.push_back(nuevo_bloque);
                    bloque_actual = &bloques_relacion.back();
                    insertarRegistro(*bloque_actual, linea_formateada, tam_registro, tam_sector);
                    registros_insertados++;
                    cout << "Nuevo bloque asignado para la relación" << endl;
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

    // Último registro si no termina con \n y aún hay espacio y no se llegó a N
    if (!campo_actual.empty() && registros_insertados < N) {
        linea_formateada += formatear_campo(campo_actual, tipos[campo_index], tamanos[campo_index]) + '\n';

        bool insertado = false;
        for (auto& bloque : bloques_relacion) {
            if (insertarRegistro(bloque, linea_formateada, tam_registro, tam_sector)) {
                insertado = true;
                registros_insertados++;
                break;
            }
        }

        if (!insertado) {
            Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre,"fija");
            if (nuevo_bloque.get_espacioDisponible() >= tam_registro) {
                bloques_relacion.push_back(nuevo_bloque);
                insertarRegistro(bloques_relacion.back(), linea_formateada, tam_registro, tam_sector);
                registros_insertados++;
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

    cout << "Carga finalizada. Registros insertados: " << registros_insertados 
         << " / " << N << ". Bloques utilizados: " << bloques_relacion.size() << endl;
}

bool verificarExistencia(vector<Bloque>& schemas, const string &tabla, string& cab, DiscoManager &manager, int &tam_sector) {
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
    string cabecera = cabecera_CSV(archivo, tabla);
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
void Delete_where(string cabecera, vector<Bloque>& Relacion, string atributo, string valor, string comparador, int tam_sector, DiscoManager &manager) {
    vector<string> nombres, tipos;
    vector<int> tamanos;

    istringstream ss(cabecera);
    string token;
    int index = 0;

    while (getline(ss, token, '#')) {
        if (index == 0) {
        } 
        else if (index % 3 == 1) {
            nombres.push_back(token);
        } 
        else if (index % 3 == 2) {
            tipos.push_back(token);
        } 
        else if (index % 3 == 0) {
            tamanos.push_back(stoi(token));
        }
        index++;
    }

    int tam_registro = calcular_tamano_registro(tamanos);
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
    for (Bloque& bloque : Relacion) {
        string linea = bloque.get_cabecera();
        Cabecera_fija cab(linea);
        int total_registros = bloque.getCantidad_Sectores() * (tam_sector / tam_registro);

        for (int pos_global = 0; pos_global < total_registros; ++pos_global) {
            string registro = bloque.getContenido_en_posicion(pos_global, tam_registro);

            if (registro.empty() || registro[0] == '*') continue;

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

            if (campo_objetivo < campos.size() &&
                func_comparador(tipos[campo_objetivo], valor, campos[campo_objetivo], comparador)) {

                stringstream nuevo;
                nuevo << "*" << setw(10) << setfill('_') << cab.inicio_lista_eliminada;
                string marcador = nuevo.str();

                if (marcador.size() < tam_registro - 1) {
                    marcador += string(tam_registro - 1 - marcador.size(), '_');
                } else if (marcador.size() > tam_registro - 1) {
                    marcador = marcador.substr(0, tam_registro - 1);
                }

                marcador += '\n';
                int sector_idx;
                bloque.setContenido_en_posicion(pos_global, marcador, tam_registro,sector_idx);
                
                stringstream nueva_cabecera;
                nueva_cabecera << setw(10) << setfill('_') << pos_global;
                nueva_cabecera << linea.substr(10);
                bloque.set_cabecera(nueva_cabecera.str());

                bloque.set_espacioDisponible(bloque.get_espacioDisponible() + tam_registro);

                cout << "[INFO] Registro eliminado en : " << bloque.getSector(sector_idx) << endl;
                manager.guardarEnDisco(bloque);
                bloque.exportarTXTbloque(0,"Eliminado");
            }
        }
    }
}

void insertarRegistroFijoDesdeTeclado(DiscoManager& manager, int sectoresPorBloque, int& tam_sector, const string& nombre, const string& cabecera) {
    vector<int> tamanos;
    vector<string> tipos;
    vector<string> nombres;
    istringstream ss(cabecera);
    string token;
    int index = 0;

    // Parsear cabecera
    while (getline(ss, token, '#')) {
        if (index == 0) {
            // Nombre de la relación (ya se tiene)
        } else if (index % 3 == 1) {
            nombres.push_back(token);
        } else if (index % 3 == 2) {
            tipos.push_back(token);
        } else if (index % 3 == 0) {
            tamanos.push_back(stoi(token));
        }
        index++;
    }

    int tam_registro = calcular_tamano_registro(tamanos);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "=== Ingrese los datos del nuevo registro para la relación '" << nombre << "' ===" << endl;
    string linea_formateada;

    for (size_t i = 0; i < nombres.size(); ++i) {
        string entrada;

        while (true) {
            cout << nombres[i] << " (" << tipos[i] << ", max " << tamanos[i] << " chars): ";
            getline(cin, entrada);

            if ((int)entrada.size() <= tamanos[i]) {
                break;
            } else {
                cout << "⚠️  Entrada demasiado larga. Máximo permitido: " << tamanos[i] << " caracteres." << endl;
            }
        }

        linea_formateada += formatear_campo(entrada, tipos[i], tamanos[i]);
        if (i != nombres.size() - 1) linea_formateada += "#";
    }

    linea_formateada += '\n';

    // Buscar bloques de la relación
    vector<Bloque> bloques_relacion;
    manager.DevolverBloquesRelacion(nombre, bloques_relacion, "fija");

    int bloques_cont = 0;
    bool insertado = false;

    for (auto& bloque : bloques_relacion) {
        if (insertarRegistro(bloque, linea_formateada, tam_registro, tam_sector)) {
            insertado = true;
            cout << " Registro insertado en bloque existente." << endl;
            break;
        }
    }

    if (!insertado) {
        Bloque nuevo_bloque = manager.Ubicar_BloqueVacio(nombre, "fija");
        if (nuevo_bloque.get_espacioDisponible() >= tam_registro) {
            insertarRegistro(nuevo_bloque, linea_formateada, tam_registro, tam_sector);
            manager.guardarEnDisco(nuevo_bloque);
            nuevo_bloque.exportarTXTbloque(bloques_cont++, nombre);
            cout << " Registro insertado en nuevo bloque." << endl;
        } else {
            cerr << " Error: No hay espacio suficiente para el nuevo registro." << endl;
        }
    } else {
        for (auto& bloque : bloques_relacion) {
            manager.guardarEnDisco(bloque);
            bloque.exportarTXTbloque(bloques_cont++, nombre);
        }
    }

    cout << " Registro procesado y almacenado correctamente." << endl;
}
