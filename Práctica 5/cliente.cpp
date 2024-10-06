#include <iostream>
#include <thread>
#include <fstream>
#include <mutex>
#include <time.h>
#include <condition_variable>
#include <string>
#include <cstring>
#include <regex>

#include "Socket.hpp"

using namespace std; 

const int TAMANIO_MENSAJE = 100;
const int N_EST = 60; //# de estudiantes
const int N_FIL = N_EST /2; //# de filas en la matriz
const int N_COL = 1000; //# de columnas

//  Pre : <fila > es un �ndice de fila de <D>
//  Post : devuelve el m�ximo de la fila <fila >
int maxFila (int D[ N_FIL ][ N_COL ] , int fila ) {

    int max = D[fila][0];

    for (int i = 1; i < N_COL; i++){

        if (D[fila][i] > max){
            max = D[fila][i];
        }
    }
    return max;
}


//  Pre : <fila > es un �ndice de fila de <D>
//  Post : devuelve la suma de los els . de la fila <fila >
int sumaFila ( int D[ N_FIL ][ N_COL ] , int fila ) {

    int suma = 0;

    for (int i = 0; i < N_COL; i++){
        suma = D[fila][i] + suma;
    }

    return suma;

}

void cargar(int D[N_FIL][N_COL]){
    
    ifstream f;
    f.open("datos.txt");
    if (f.is_open()){
        int dato_n;
        int i=0;
        int j=0;
        while(i<N_FIL){
            f>>dato_n;
            D[i][j]=dato_n;    //transferencia de datos al vector
            j++;
            if (j==N_COL){
                j=0;
                i++;
            }
        }
        f.close();
    }
    else{
        cerr<<"No se encontro el archivo de datos.txt";
    }
}

void recibir_mensaje (Socket &chan, int cliente, string& mensaje){


    int bytes_recibidos = chan.Receive(cliente, mensaje, TAMANIO_MENSAJE);

    if( bytes_recibidos == -1){

        cerr << chan.error("Error; Receive", errno);
        exit(1);

    }

}

void enviar_mensaje (Socket &chan, int cliente, string mensaje){


    int bytes_enviados = chan.Send(cliente, mensaje);

    if(bytes_enviados == -1){

        cerr << chan.error("Error; Send", errno);
        exit(1);

    }

}

void trocea(string mensaje, int& dato1, int& dato2){

    char* token;
    char* copia = strdup(mensaje.c_str());

    token = strtok(copia,",");
    dato1 = atoi(token);

    token = strtok(nullptr,",");
    dato2 = atoi(token);

    free (copia);

}

void Estudiante(int nip, int D[N_FIL][N_COL], int puerto){

    string ip = "localhost";

    Socket chan(ip, puerto);

    int socket;

    do {

        socket = chan.Connect();

        if(socket == -1){

            this_thread::sleep_for(chrono::seconds(1));

        }

    }while(socket == -1);

    string mensaje;

    mensaje = "sentar," + to_string(nip);

    enviar_mensaje(chan, socket, mensaje);

    recibir_mensaje(chan, socket, mensaje);
    
    int fila, pareja;
    trocea(mensaje,pareja,fila);

    if(nip < pareja){

        int maximo = maxFila(D, fila);
        mensaje = "max," + to_string(maximo);
        enviar_mensaje(chan, socket, mensaje);

    }
    else{

        int suma = sumaFila(D, fila);
        mensaje = "suma," + to_string(suma);
        enviar_mensaje(chan, socket, mensaje);
    }

    chan.Close();

}



int main(int argc, char* argv[]){

    int puerto = atoi(argv[1]);

    int D[N_FIL][N_COL];

    cargar(D);

    thread E[N_EST];
    for (int i = 0; i < N_EST; i++){

        E[i] = thread(&Estudiante, i, D, puerto);

    }

    for (int i = 0; i < N_EST; i++){

        E[i].join();
        
    }

    return 0 ;

}
