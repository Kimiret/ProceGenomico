#include <iostream>
#include <thread>
#include <fstream>
#include <cctype>
#include <filesystem>
#include <string>
#include <vector>
#include <queue>
#include <semaphore.h>

using namespace std;

const string directorio_base = "/mnt/c/Users/Ralsei/Desktop/Ramos/Sistemas Operativos/Tarea2/ProceGenomico/";

sem_t semaforo_cola;
queue<string> cola_archivos;

void en_umbral(filesystem::path ruta, double umbral){
    ifstream archivo(ruta);
    if (archivo.is_open()) {
        //cout << "Procesando " << ruta.filename() << endl;
        
        // Variables para contar los caracteres
        int contadorA = 0, contadorT = 0, contadorC = 0, contadorG = 0;

        string linea;
        while (getline(archivo, linea)) {
            // Excluir las líneas que comienzan con '>'
            if (linea.size() > 0 && linea[0] == '>') {
                continue;
            }

            // Procesar cada caracter de la línea
            for (char caracter : linea) {
                if (isalpha(caracter)) {
                    // Contar la ocurrencia del caracter tomando en cuenta posibles instancias de caracteres en minuscula 
                    caracter = toupper(caracter);
                    switch (caracter) {
                        case 'A':
                            contadorA++;
                            break;
                        case 'T':
                            contadorT++;
                            break;
                        case 'C':
                            contadorC++;
                            break;
                        case 'G':
                            contadorG++;
                            break;
                        default:
                            // Caracter no reconocido
                            break;
                    }
                }
            }
        }

        

        archivo.close();

        //Se procesa la info del archivo y se calcula la proporcion CG, para ver si cumple con el umbral
        sem_wait(&semaforo_cola);
        
        /*cout << "Contador de A: " << contadorA << endl;
        cout << "Contador de T: " << contadorT << endl;
        cout << "Contador de C: " << contadorC << endl;
        cout << "Contador de G: " << contadorG << endl;*/
        double total = contadorA + contadorT + contadorC + contadorG; 
        double proporcion = contadorC + contadorG;
        proporcion = proporcion/total;
        /*cout << "Archivo: " << ruta.filename() << endl;
        cout << "Proporcion de contenido genetico CG: " << proporcion << endl;*/
        if(proporcion >= umbral) cola_archivos.push(ruta.filename());
        
        sem_post(&semaforo_cola);
    } else {
        cerr << "No se pudo abrir el archivo." << endl;
    }
}

int main(){

    //Proceso simple que checkea que el directorio y umbral dados por el usuario sean utiles
    string directorio;
    double umbral = -1;
    do{
        string aux;
        cout << "Ingrese un directorio valido para procesar: ";
        cin >> aux ;
        directorio = directorio_base + aux;
    }while(!filesystem::exists(directorio) || !filesystem::is_directory(directorio));

    do{
        cout << "Ingrese un valor de umbral entre 0 y 1 para filtrar genomas: ";
        cin >> umbral;
    }while(umbral<=0 || umbral>=1);

    //Se inicializa el semaforo
    sem_init(&semaforo_cola,0,1);

    vector<thread> hebras;
    int num_hebras = 4;
    //Segmento donde se procesa cada archivo dentro del directorio dado, excluyendo subdirectorios
    for(const auto archivo : filesystem::directory_iterator(directorio)){
        if(!filesystem::is_directory(archivo)){
            hebras.emplace_back(en_umbral,archivo.path(),umbral);

            //Esto limita el numero de hebras activas a la variable num_hebras
            if(hebras.size() == num_hebras){
                for(auto& t : hebras) {
                    t.join();
                }
                hebras.clear();
            }
        } 
    }

    for(auto& t: hebras){
        t.join();
    }

    cout << "Archivos de genomas que superaron el umbral:" << endl;
    while(!cola_archivos.empty()){
        cout << cola_archivos.front() << endl;
        cola_archivos.pop();
    };

    sem_destroy(&semaforo_cola);


    return 0;
}