#include <iostream>
#include <cstdlib>
#include <cctype>
#include <algorithm>
using namespace std;
bool comparador_int(const string valor, const string valor_B, const string comparador){
    int a=stoi(valor);
    int b=stoi(valor_B);
    if(comparador=="="){
        return a==b;
    }
    else if(comparador==">="){
        return a>=b;
    }
    else if(comparador=="!="){
        return a!=b;
    }
    else if(comparador=="<="){
        return a<=b;
    }
    else if(comparador=="<"){
        return a<b;
    }
    else if(comparador==">"){
        return a>b;
    }
    return false;
}

bool comparador_str(const string valor, const string valor_B, const string comparador){
    string a=valor;
    string b=valor_B; 
    if(comparador=="="){
        return a==b;
    }
    else if(comparador==">="){
        return a>=b;
    }
    else if(comparador=="!="){
        return a!=b;
    }
    else if(comparador=="<="){
        return a<=b;
    }
    else if(comparador=="<"){
        return a<b;
    }
    else if(comparador==">"){
        return a>b;
    }
    return false;
}

bool comparador_float(const string valor, const string valor_B, const string comparador){
    float a=stof(valor);
    float b=stof(valor_B);
    if(comparador=="="){
        return a==b;
    }
    else if(comparador==">="){
        return a>=b;
    }
    else if(comparador=="!="){
        return a!=b;
    }
    else if(comparador=="<="){
        return a<=b;
    }
    else if(comparador=="<"){
        return a<b;
    }
    else if(comparador==">"){
        return a>b;
    }
    return false;
}

bool func_comparador(const string tipo, const string valor, const string valor_B, const string comparador){
    if(tipo=="int"){
        return comparador_int(valor,valor_B,comparador);
    }
    else if(tipo=="str"){
        return comparador_str(valor,valor_B,comparador);
    }else if(tipo=="float"){
        return comparador_float(valor,valor_B,comparador);
    }
    return false;
}

bool esEntero(const string& str) {
    if (str.empty()) return false;
    int i = 0;
    if (str[0] == '-' || str[0] == '+') i++;
    if (i == str.size()) return false; 

    for (; i < str.size(); i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

bool esDecimal(const string& str) {
    if (str.empty()) return false;
    int i = 0;
    bool punto = false;

    if (str[0] == '-' || str[0] == '+') i++;
    if (i == str.size()) return false; 

    for (; i < str.size(); i++) {
        if (str[i] == '.') {
            if (punto) return false; 
            punto = true;
        } else if (!isdigit(str[i])) {
            return false;
        }
    }
    return punto;
}
bool revision_tipo(const string& tipo,const string& valor){
    if(tipo=="int"){
        return esEntero(valor);
    }else if(tipo=="str"){
        return true;
    }else if(tipo=="float"){
        return esDecimal(valor);
    }   
    return false;
}

string detectarTipo(const string& valor){
    if(esEntero(valor)){
        return "int";
    }else if(esDecimal(valor)){
        return "float";
    }else{
        return "str";
    }
}

string limpiarpalabra(string valor){
    string palabra;
    for(int i=0; i<valor.size(); i++){
        if(valor[i]!='_' && valor[i]!='\n'){
            palabra+=valor[i];
        }
    }
    return palabra;
}

void cordenadas(const string& linea, int& plato, int& superficie, int& pista, int& sector) {
    try {
        string str_plato      = linea.substr(0, 10);
        string str_superficie = linea.substr(10, 10);
        string str_pista      = linea.substr(20, 10);
        string str_sector     = linea.substr(30, 10);

        str_plato.erase(remove(str_plato.begin(), str_plato.end(), '_'), str_plato.end());
        str_superficie.erase(remove(str_superficie.begin(), str_superficie.end(), '_'), str_superficie.end());
        str_pista.erase(remove(str_pista.begin(), str_pista.end(), '_'), str_pista.end());
        str_sector.erase(remove(str_sector.begin(), str_sector.end(), '_'), str_sector.end());

        plato      = stoi(str_plato);
        superficie = stoi(str_superficie);
        pista      = stoi(str_pista);
        sector     = stoi(str_sector);
    } catch (const exception& e) {
        cerr << "Error al leer coordenadas: " << e.what() << endl;
        plato = superficie = pista = sector = -1;
    }
}
