/*
 * Autores: Javier Julve Yubero, Alejandro Benedi Andres
 * NIP´s: 840710, 843826
*/

#include "ControlParejas.hpp"

void monitor::constructor(){

    sillas_ocupadas = 0;
    contador = 0;

    for (int i = 0; i < N_EST; i++){

        calculado[i] = 0;
        datos_pasados[i] = 0;
        NIPS[i] = 0;
        pareja[i] = 0;
        filas[i] = 0;
        maximo[i] = 0;

    }
}


void monitor::sillas_libres(int nip){

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


void monitor::asignacion(int nip, int& devolver_fila, int& devolver_pareja){

    unique_lock<mutex> lck(mtx_monitor);

    while(!datos_pasados[nip]){
        c_datos_pasado[nip].wait(lck);
    }

    devolver_fila = filas[nip];
    devolver_pareja = pareja[nip];


}


void monitor::fin_pareja(int nip, int& devolver_maximo){

    unique_lock<mutex> lck(mtx_monitor);
    
    while(!calculado[pareja[nip]]){
        c_calculado[nip].wait(lck);
    }
    
    devolver_maximo = maximo[pareja[nip]];
    
}


void monitor::terminado(){

    unique_lock<mutex> lck(mtx_monitor);

    while(contador < N_FIL){
        c_terminado.wait(lck);
    }
}


void monitor::desbloquear_fin_pareja(int nip, int resultado){

    unique_lock<mutex> lck(mtx_monitor);

    calculado[nip] = true;
    c_calculado[pareja[nip]].notify_one();

    maximo[nip] = resultado;
}


void monitor::desbloquear_terminado(){

    unique_lock<mutex> lck(mtx_monitor);

    contador++;

    if(contador >= N_FIL){
        c_terminado.notify_one();
    }
}


void monitor::hay_estudiantes(int fila){

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
