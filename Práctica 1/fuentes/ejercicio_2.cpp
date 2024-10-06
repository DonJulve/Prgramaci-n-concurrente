//------------------------------------------------------------------------------
// File:   ejercicio_2.cpp
// Author: Javier Julve, Alejandro Benedí
// Date:   septiembre 2022
// Coms:   Parte del práctica 1 de PSCD
//------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <time.h>

using namespace std;

void numeritos(int num, int retardo, int veces) {
    for(int i=0; i<=veces; i++) {
        cout << "Soy " + to_string(num+1) + "\n";
        this_thread::sleep_for(chrono::milliseconds(retardo));
    }
}


int aleatorio(int inicial,int final){
    srand(time(nullptr));
    return inicial+rand()%((final+1)-inicial);
}

int main(int argc, char* argv[]){
    int Max=10;
    thread P[Max];
    for(int i=0;i<Max;i++){
        
        P[i] = thread(&numeritos,i,aleatorio(100,300),aleatorio(5,15));
    }
     for(int i=0;i<Max;i++){
        
        P[i].join(); 
    }
}