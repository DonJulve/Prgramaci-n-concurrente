//------------------------------------------------------------------------------
// File:   practica_1_V2.cpp
// Author: PSCD-Unizar
// Date:   septiembre 2020
// Coms:   Parte del práctica 1 de PSCD
//         Compilar mediante
//           g++ practica_1_V2.cpp -o practica_1_V2 -std=c++11 -pthread
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
    const int N = 3;
    thread P[N]; //de momento, ningún thread se pone en marcha

    P[0] = thread(&saludo, "Aurora", 100, 10); //P[0] se pone en marcha
    P[1] = thread(&saludo, "Baltasar", 150, 15),
    P[2] = thread(&saludo, "Carmen", 300, 5);

    P[0].join(); //me bloqueo hasta que "P[0]" termine
    P[1].join();
    P[2].join();
    // alternativamente, podíamos haber ejecutado
    // for (int i=0; i<N; i++) {
    //     P[i].join();
    // }

    cout << "Fin\n";
    return 0;
}
