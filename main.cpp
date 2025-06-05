#include "reg_variable.h"

void mostrarMenu() {
    cout << "\n===== Menu Principal =====" << endl;
    cout << "1. Crear disco" << endl;
    cout << "2. Select From * L.Fija" << endl;
    cout << "3. Cargar todo CSV L.fija" << endl;
    cout << "4. Agregar N registros L.fija" << endl;
    cout << "5. Salir" << endl;
    cout << "6. Select From Where" <<endl;
    cout << "7. Eliminar un registro "<<endl;
    cout << "8. Mostrar Bloques Libres" <<endl;
    cout << "9. Mostrar Bloques Ocupados" <<endl;
    cout << "10. Cargar todo CSV L.Variable" <<endl;
    cout << "11. Agregar N registros L.Variable" <<endl;
    cout << "12. SELECT FROM* L.Variable" <<endl;
    cout << "13. Eliminar un registo L.Variable" <<endl;
    cout << "14. Mostrar Sectores Ocupados" <<endl;
    cout << "15. Ingresar 1 Registro L.Variable" <<endl;
    cout << "16. Ingresar 1 Registro L.Fija" <<endl;
    cout << "Seleccione una opcion: ";
}

void ejecutarInstruccion(int opcion, Disco &d, bool &discoCreado, int &tamSector, int &sectoresPorBloque, DiscoManager* &manager) {
    int tam_bytes_sector;
    if (opcion == 1) {
        d = crearDisco();
        tamSector = d.getTamSector();
        sectoresPorBloque = d.getSectoresPorBloque();
        discoCreado = true;
        cout << "Disco creado exitosamente." << endl;
        tam_bytes_sector = tamSector;
        if (manager != nullptr) delete manager;
        manager = new DiscoManager(d, sectoresPorBloque, tam_bytes_sector); 
    }
    else if (opcion == 2) {
        if (!discoCreado || manager == nullptr) {
            cout << "Primero debes crear el disco (opción 1)." << endl;
            return;
        }
        string Tabla;
        cout << "Nombre Archivo: ";
        cin >> Tabla;
        vector<Bloque> schemas;
        manager->DevolverBloquesRelacion("Schemas",schemas,"fija");
        string cabecera;
        vector<Bloque> Relacion;
        if(verificarExistencia(schemas,Tabla,cabecera,*manager,tamSector)){
            manager->DevolverBloquesRelacion(Tabla,Relacion,"fija");
        }else{
            cout<<"No existe la relacion: "<<Tabla<<endl;
        }
        Select_Todo(cabecera,Relacion);
    }
    else if (opcion == 3) {
        if (!discoCreado || manager == nullptr) {
            cout << "Primero debes crear el disco (opción 1)." << endl;
            return;
        }

        if (tamSector <= 0) {
            cerr << "Error: tamaño de sector no válido." << endl;
            return;
        }

        string Tabla;
        cout << "Nombre Archivo: ";
        cin >> Tabla;
        vector<Bloque> schemas;
        manager->DevolverBloquesRelacion("Schemas",schemas,"fija");
        string cabecera;
        vector<Bloque> Relacion;
        verificarExistencia(schemas,Tabla,cabecera,*manager,tamSector);
        fstream archivo(Tabla + ".csv", ios::in);
        if (!archivo.is_open()) {
            cerr << "Error: No se pudo abrir el archivo CSV '" << Tabla << ".csv'" << endl;
            return;
        }
        archivo.seekg(0, ios::beg);
        contenido_total(archivo, cabecera, *manager, sectoresPorBloque, tamSector, Tabla);
        d.mostrar_estado_actual();
    }
    else if (opcion == 4) {
        if (!discoCreado || manager == nullptr) {
            cout << "Primero debes crear el disco (opción 1)." << endl;
            return;
        }
        if (!discoCreado || manager == nullptr) {
            cout << "Primero debes crear el disco (opción 1)." << endl;
            return;
        }

        if (tamSector <= 0) {
            cerr << "Error: tamaño de sector no válido." << endl;
            return;
        }
        int  N;
        string Tabla;
        cout << "Nombre Archivo: ";
        cin >> Tabla;
        cout << "Cantidad Registros: ";
        cin >> N;
        vector<Bloque> schemas;
        manager->DevolverBloquesRelacion("Schemas",schemas,"fija");
        string cabecera;
        vector<Bloque> Relacion;
        verificarExistencia(schemas,Tabla,cabecera,*manager,tamSector);
        fstream archivo(Tabla + ".csv", ios::in);
        if (!archivo.is_open()) {
            cerr << "Error: No se pudo abrir el archivo CSV '" << Tabla << ".csv'" << endl;
            return;
        }
        archivo.seekg(0, ios::beg);
        contenido_parcial(archivo, cabecera, *manager, sectoresPorBloque, tamSector, Tabla, N);
        d.mostrar_estado_actual();
    }
    else if (opcion == 5) {
        cout << "Saliendo del programa..." << endl;
    }
    else if (opcion == 6) {
        string Tabla,comparador,atributo,valor;
        cout << "Nombre Archivo: ";
        cin >> Tabla;
        cout << "Comparador: ";
        cin >> comparador;
        cout << "Atributo: ";
        cin>> atributo;
        cout << "Valor: ";
        cin>> valor;
        vector<Bloque> schemas;
        manager->DevolverBloquesRelacion("Schemas",schemas,"fija");
        string cabecera;
        vector<Bloque> Relacion;
        if(verificarExistencia(schemas,Tabla,cabecera,*manager,tamSector)){
            manager->DevolverBloquesRelacion(Tabla,Relacion,"fija");
        }else{
            cout<<"No existe la relacion: "<<Tabla<<endl;
        }
        Select_where(cabecera,Relacion,atributo,valor,comparador);
    }
    else if(opcion == 7){
        string Tabla,comparador,atributo,valor;
        cout << "Nombre Archivo: ";
        cin >> Tabla;
        cout << "Comparador: ";
        cin >> comparador;
        cout << "Atributo: ";
        cin>> atributo;
        cout << "Valor: ";
        cin>> valor;
        vector<Bloque> schemas;
        manager->DevolverBloquesRelacion("Schemas",schemas,"fija");
        string cabecera;
        vector<Bloque> Relacion;
        if(verificarExistencia(schemas,Tabla,cabecera,*manager,tamSector)){
            manager->DevolverBloquesRelacion(Tabla,Relacion,"fija");
        }else{
            cout<<"No existe la relacion: "<<Tabla<<endl;
        }
        Delete_where(cabecera,Relacion,atributo,valor,comparador,tamSector,*manager);
    }
    else if(opcion==8){
        manager->mostrar_BloqueVacio();
    }
    else if(opcion==9){
        manager->mostrar_BloquesOcupados();
    }else if(opcion==10){
        if (!discoCreado || manager == nullptr) {
            cout << "Primero debes crear el disco (opción 1)." << endl;
            return;
        }

        if (tamSector <= 0) {
            cerr << "Error: tamaño de sector no válido." << endl;
            return;
        }

        string Tabla;
        cout << "Nombre Archivo: ";
        cin >> Tabla;
        vector<Bloque> schemas;
        manager->DevolverBloquesRelacion("Schemas",schemas,"fija");
        string cabecera;
        vector<Bloque> Relacion;
        verificarExistencia_var(schemas,Tabla,cabecera,*manager,tamSector);
        fstream archivo(Tabla + ".csv", ios::in);
        if (!archivo.is_open()) {
            cerr << "Error: No se pudo abrir el archivo CSV '" << Tabla << ".csv'" << endl;
            return;
        }
        archivo.seekg(0, ios::beg);
        contenido_total_var(archivo, cabecera, *manager, sectoresPorBloque, tamSector, Tabla);
        d.mostrar_estado_actual();
    }else if(opcion==11){
        if (!discoCreado || manager == nullptr) {
            cout << "Primero debes crear el disco (opción 1)." << endl;
            return;
        }

        if (tamSector <= 0) {
            cerr << "Error: tamaño de sector no válido." << endl;
            return;
        }

        string Tabla;
        cout << "Nombre Archivo: ";
        cin >> Tabla;
        int N;
        cout << "Cantidad de Archivos: ";
        cin>>N; 
        vector<Bloque> schemas;
        manager->DevolverBloquesRelacion("Schemas",schemas,"fija");
        string cabecera;
        vector<Bloque> Relacion;
        verificarExistencia_var(schemas,Tabla,cabecera,*manager,tamSector);
        fstream archivo(Tabla + ".csv", ios::in);
        if (!archivo.is_open()) {
            cerr << "Error: No se pudo abrir el archivo CSV '" << Tabla << ".csv'" << endl;
            return;
        }
        archivo.seekg(0, ios::beg);
        contenido_parcial_var(archivo, cabecera, *manager, sectoresPorBloque, tamSector, Tabla,N);
        d.mostrar_estado_actual();
    }
    else if(opcion==12){
        if (!discoCreado || manager == nullptr) {
            cout << "Primero debes crear el disco (opción 1)." << endl;
            return;
        }
        string Tabla;
        cout << "Nombre Archivo: ";
        cin >> Tabla;
        vector<Bloque> schemas;
        manager->DevolverBloquesRelacion("Schemas",schemas,"fija");
        string cabecera;
        vector<Bloque> Relacion;
        if(verificarExistencia_var(schemas,Tabla,cabecera,*manager,tamSector)){
            manager->DevolverBloquesRelacion(Tabla,Relacion,"variable");
        }else{
            cout<<"No existe la relacion: "<<Tabla<<endl;
        }
        Select_Todo_VAR(cabecera,Relacion);
    }else if(opcion==13){
        string Tabla,comparador,atributo,valor;
        cout << "Nombre Archivo: ";
        cin >> Tabla;
        cout << "Comparador: ";
        cin >> comparador;
        cout << "Atributo: ";
        cin>> atributo;
        cout << "Valor: ";
        cin>> valor;
        vector<Bloque> schemas;
        manager->DevolverBloquesRelacion("Schemas",schemas,"fija");
        string cabecera;
        vector<Bloque> Relacion;
        if(verificarExistencia_var(schemas,Tabla,cabecera,*manager,tamSector)){
            manager->DevolverBloquesRelacion(Tabla,Relacion,"variable");
        }else{
            cout<<"No existe la relacion: "<<Tabla<<endl;
        }
        Delete_where_VAR(cabecera,Relacion,atributo,valor,comparador,tamSector,*manager);
    }else if(opcion==14){
        manager->mostrar_SectoresOcupados();
    }else if(opcion==15){
        string Tabla;
        cout << "Nombre Archivo: ";
        cin >> Tabla;
        vector<Bloque> schemas;
        manager->DevolverBloquesRelacion("Schemas",schemas,"fija");
        string cabecera;
        vector<Bloque> Relacion;
        verificarExistencia_var(schemas,Tabla,cabecera,*manager,tamSector);
        fstream archivo(Tabla + ".csv", ios::in);
        if (!archivo.is_open()) {
            cerr << "Error: No se pudo abrir el archivo CSV '" << Tabla << ".csv'" << endl;
            return;
        }
        archivo.seekg(0, ios::beg);
        insertarRegistroDesdeTeclado_VAR( *manager, sectoresPorBloque, tamSector, Tabla, cabecera);
        d.mostrar_estado_actual();
    }else if(opcion==16){
        string Tabla;
        cout << "Nombre Archivo: ";
        cin >> Tabla;
        vector<Bloque> schemas;
        manager->DevolverBloquesRelacion("Schemas",schemas,"fija");
        string cabecera;
        vector<Bloque> Relacion;
        verificarExistencia(schemas,Tabla,cabecera,*manager,tamSector);
        fstream archivo(Tabla + ".csv", ios::in);
        if (!archivo.is_open()) {
            cerr << "Error: No se pudo abrir el archivo CSV '" << Tabla << ".csv'" << endl;
            return;
        }
        archivo.seekg(0, ios::beg);
        insertarRegistroFijoDesdeTeclado(*manager, sectoresPorBloque, tamSector, Tabla, cabecera);
        d.mostrar_estado_actual();
    }
    else {
        cout << "Opción inválida. Intente de nuevo.\n";
    }
}

int main() {
    int opcion;
    Disco d;
    bool discoCreado = false;
    int tamSector = 0;
    int sectoresPorBloque = 0;
    DiscoManager* manager = nullptr;
    do {
        mostrarMenu();
        cin >> opcion;
        ejecutarInstruccion(opcion, d, discoCreado, tamSector, sectoresPorBloque, manager);
    } while (opcion != 5);
    return 0;
}
