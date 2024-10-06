/*
 * Autores: Javier Julve Yubero, Alejandro Benedi Andres
 * NIP´s: 840710, 843826
*/

#include "ControlParejas.hpp"

void monitor::constructor(){

    unique_lock<mutex> lck(mtx_monitor);

    sillas_ocupadas = 0;
    contador = 0;

    for (int i = 0; i < N_EST; i++){

        calculado[i] = 0;
        datos_pasados[i] = 0;

    }
}


void monitor::sillas_libres(int nip, int NIPS[]){

    unique_lock<mutex> lck(mtx_monitor);

    while(sillas_ocupadas >= 2){
        c_sillas_menor_2.wait(lck);
    }
    
    NIPS[sillas_ocupadas] = nip;
    sillas_ocupadas++;

    if(sillas_ocupadas >= 2){
        c_sillas_igual_2.notify_one();
    }

}

void monitor::asignacion(int nip){

    unique_lock<mutex> lck(mtx_monitor);

    while(!datos_pasados[nip]){
        c_datos_pasado[nip].wait(lck);
    }

}

void monitor::fin_pareja(int nip, int pareja[]){

    unique_lock<mutex> lck(mtx_monitor);
    
    while(!calculado[pareja[nip]]){
        c_calculado[nip].wait(lck);
    }
}

void monitor::terminado(){

    unique_lock<mutex> lck(mtx_monitor);

    while(contador < N_FIL){
        c_terminado.wait(lck);
    }
}

void monitor::desbloquear_fin_pareja(int nip, int pareja[]){

    unique_lock<mutex> lck(mtx_monitor);

    calculado[nip] = true;
    c_calculado[pareja[nip]].notify_one();
}

void monitor::desbloquear_terminado(){

    unique_lock<mutex> lck(mtx_monitor);

    contador++;

    if(contador >= N_FIL){
        c_terminado.notify_one();
    }
}

void monitor::hay_estudiantes(int pareja[], int NIPS[], int filas[], int fila){

    unique_lock<mutex> lck(mtx_monitor);

    while(sillas_ocupadas < 2){
        c_sillas_igual_2.wait(lck);
    }

    pareja[NIPS[0]] = NIPS[1];
    pareja[NIPS[1]] = NIPS[0];

    for (int i = 0; i < 2; i++){
        filas[NIPS[i]] = fila;
        datos_pasados[NIPS[i]] = true;
        c_datos_pasado[NIPS[i]].notify_one();
        c_sillas_menor_2.notify_one();
    }

    sillas_ocupadas = 0;

}
