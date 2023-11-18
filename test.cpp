#include <iostream>
#include <thread>
#include <fstream>
#include <cctype>

using namespace std;

int main() {
    unsigned int cores = thread::hardware_concurrency();
    cout << "Número de hebras soportadas: " << cores << endl;

    ifstream archivo("genomas/GCF_000629185.1_Pseu_aeru_3580_V1_genomic.fna");

    if (archivo.is_open()) {
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
                    // Contar la ocurrencia del caracter
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

        // Mostrar los resultados
        cout << "Contador de A: " << contadorA << endl;
        cout << "Contador de T: " << contadorT << endl;
        cout << "Contador de C: " << contadorC << endl;
        cout << "Contador de G: " << contadorG << endl;
        double total = contadorA + contadorT + contadorC + contadorG; 
        double proporcion = contadorC + contadorG;
        proporcion = proporcion/total;
        cout << "Proporcion de contenido genetico CG: " << proporcion << endl;

        archivo.close();
    } else {
        cerr << "No se pudo abrir el archivo." << endl;
    }

    return 0;
}
