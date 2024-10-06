//------------------------------------------------------------------------------
// File:   practica_1_V1.cpp
// Author: PSCD-Unizar
// Date:   septiembre 2020
// Coms:   Parte del práctica 1 de PSCD
//         Compilar mediante
//           g++ practica_1_V1.cpp -o practica_1_V1 -std=c++11 -pthread
//------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <string>
#include <chrono>

using namespace std;

void saludo(string nombre, int retardo, int veces) {
    for(int i=1; i<=veces; i++) {
        // cout << "Soy " << nombre << endl;
        cout << "Soy " + nombre + "\n";
        //el thread que me ejecuta se bloquea durante "retardo" milisegundos
        this_thread::sleep_for(chrono::milliseconds(retardo));
    }
}

int main(int argc, char* argv[]) {
    thread th_1(&saludo, "Aurora", 100, 10);   //th_1 se pone en marcha
    thread th_2(&saludo, "Baltasar", 150, 15);
    thread th_3(&saludo, "Carmen", 300, 5);

    th_1.join(); //me bloqueo hasta que "th_1" termine
    th_2.join();
    th_3.join();

    cout << "Fin\n";
    return 0;
}
