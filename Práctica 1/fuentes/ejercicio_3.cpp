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
#include <math.h>

using namespace std;

const int NUM_DATOS = 100;
const int NUM_PROCESOS = 3;

bool mediaEncontrada = false;  


/*
*   Pre:  ---
*   Post: Devuelve un numero aleatorio de tipo float comprendido entre <inicial> y <final>
*/
float aleatorio(const float inicial, const float final){
    return inicial + float(rand() / float (RAND_MAX/(final - inicial)));
}

/*
*   Pre:  ---
*   Post: Este proceso calcula la media aritmetica de todos los elementos del vector <reales>
*       y lo devuelve a traves de <resultado>
*/
void Media(float reales[], float& resultado){

    for(int i=0; i<NUM_DATOS; i++){
        resultado=+reales[i];
    }
    resultado=resultado/NUM_DATOS;
}

/*
*   Pre:  ---
*   Post: Este proceso calcula el valor maximo y minimo de todos los elementos del vector <reales>
*       y lo devuelve a traves de <max> y <min>
*/
void maxymin(float reales[], float& max, float& min){
    
    max=reales[0];
    min=reales[0];
    for(int i=1; i<NUM_DATOS; i++){
        
        if(reales[i]>max){
            max=reales[i];
        }
        
        if(reales[i]<min){
            min=reales[i];
        }
    }

    mediaEncontrada = true;
}

/*
*   Pre:  ---
*   Post: Este proceso devuelve la desviacion típica de todos los elementos del vector <reales>
*       y lo devuelve a traves de <desviacion>
*/
void sigma(float reales[], float media, float& desviacion){

    while(!mediaEncontrada){

    }

    for (int i=0; i<NUM_DATOS; i++){
        desviacion = desviacion + pow((reales[i] - media),2);
    }

    desviacion=sqrt(desviacion/NUM_DATOS);
}



int main(int argc, char* argv[]){
    
    srand(time(nullptr));

    float reales[NUM_DATOS];
    float media = 0, max = 0, min = 0, desviacion = 0;

    thread P[NUM_PROCESOS];

    for(int i=0; i < NUM_DATOS; i++){
        reales[i] = aleatorio(1,1000);
    }
    
    P[0] = thread(Media, ref(reales), ref(media));
    P[1] = thread(&maxymin, ref(reales), ref(max), ref(min));
    P[2] = thread(&sigma, ref(reales), media, ref(desviacion));

    
    for(int i = 0; i < NUM_PROCESOS; i++){
        P[i].join();
    }

    cout << "# datos: " <<  NUM_DATOS << endl;
    cout << "media: " << media << endl;
    cout << "max: " << max << endl;
    cout << "min: " << min << endl;
    cout << "sigma: " << desviacion << endl;
    
    return 0;
}