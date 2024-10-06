/*
 * Autores: Javier Julve Yubero, Alejandro Benedi Andres
 * NIP´s: 840710, 843826
*/

#include <iostream>
#include <thread>
#include <fstream>
#include <mutex>
#include <time.h>
#include <condition_variable>
#include <string>
#include <cstring>
#include <regex>

#include "ControlParejas.hpp"
#include "Socket.hpp"

using namespace std; 

const int TAMANIO_MENSAJE = 100;

//  Pre: ---
//  Post: Muestra el resultado en forma de tabla 
void mostrarResultado(int fila, int nip, int pareja, int maximo, int suma){


    cout << to_string(fila) + "   |  " + to_string(pareja) + "-" + to_string(nip) + "  |  " 
        + to_string(maximo) + "  |  " + to_string(suma) + "\n";
}


//  PROCESO PROFESOR
//  Inicialmente este proceso se mantendra a la espera hasta que se siente 2 alumnos.Tras ello el proceso
//  profesor comunicara a cada miembro de la pareja la fila que les ha tocado y el nip del otro miembro y 
//  les dejara entrar ha realizar la prueba.Tras formar las parejas el profesor se quedara bloqueado
//  hasta que todas las parejas hayan terminado la prueba.
void Profesor (monitor& mon){

    for( int i=0; i< N_FIL ; i++) {
        
        mon.hay_estudiantes(i);
        
    }

    // esperar que todos hayan terminado
    mon.terminado();
}

void dormir(){

    int retardo = (0 + rand() % (5 - 0)) * 1000;
    this_thread::sleep_for(chrono::milliseconds(retardo));              // "Duerme un numero aleatorio de milisegundos"

}


void comprobar_mensaje(string mensaje, string patron, Socket& chan, int cliente, monitor& mon){

    regex pat(patron);

    if(!regex_match(mensaje,pat)){

        cout << "El mensaje recibido no cumple con la especificacion del problema. Se va a cerrar la conexion con el cliente\n";
        chan.Close(cliente);
        mon.desbloquear_terminado();
        exit(1);
    }

}


void recibir_mensaje (Socket &chan, int cliente, string& mensaje){

    int bytes_recibidos = chan.Receive(cliente, mensaje, TAMANIO_MENSAJE);

    if( bytes_recibidos == -1){

        cerr << chan.error("Error; Receive", errno);
        chan.Close(cliente);
        exit(1);

    }

}

void enviar_mensaje (Socket &chan, int cliente, string mensaje){


    int bytes_enviados = chan.Send(cliente, mensaje);

    if(bytes_enviados == -1){

        cerr << chan.error("Error; Send", errno);
        chan.Close(cliente);
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

void Representante (monitor& mon, int cliente, Socket& chan){

    string mensaje;

    recibir_mensaje(chan, cliente, mensaje);
    
    comprobar_mensaje(mensaje, "sentar,([0-9]+)", chan, cliente, mon);

    int nip, ignorar;
    trocea(mensaje, ignorar, nip);

    mon.sillas_libres(nip);

    int pareja, fila;
    mon.asignacion(nip, fila, pareja);

    mensaje = to_string(pareja) + "," + to_string(fila);
    enviar_mensaje(chan, cliente, mensaje);

    if(nip < pareja){

        dormir();

        recibir_mensaje(chan, cliente, mensaje);

        comprobar_mensaje(mensaje, "max,([0-9]+)", chan, cliente, mon);

        int maximo;
        trocea(mensaje, ignorar, maximo);

        mon.desbloquear_fin_pareja(nip, maximo);

    }
    else{

        int maximo, suma; 

        recibir_mensaje(chan, cliente, mensaje);
        
        comprobar_mensaje(mensaje, "suma,([0-9]+)", chan, cliente, mon);

        trocea(mensaje, ignorar, suma);

        mon.fin_pareja(nip, maximo);

        mostrarResultado(fila, nip, pareja, maximo, suma);

        mon.desbloquear_terminado();

    }
    
    chan.Close(cliente);
}


int main(int argc, char* argv[]){

    monitor mon;        // Es el monitor 

    mon.constructor();

    thread P;
    P = thread(&Profesor, ref(mon));


    int SERVER_PORT = atoi(argv[1]);
    Socket chan(SERVER_PORT);

    //bind
    int socket_fd = chan.Bind();
    if(socket_fd == -1){

        cerr << chan.error("Error: Bind",errno);
        exit(1);

    }

    //listen
    int error = chan.Listen();
    if(error == -1){
        
        cerr << chan.error("Error: Listen",errno);
        exit(1);

    }


    int nuevo_cliente;
    thread R[N_EST];

    for (int i = 0; i < N_EST; i++){                // Lanza los 60 procesos representante

        nuevo_cliente = chan.Accept();
        if(nuevo_cliente == -1){

            cerr << chan.error("Error: Accept",errno);
            exit(1);
        }

        R[i] = thread(&Representante, ref(mon), nuevo_cliente, ref(chan));

    }

    P.join();                                       // Cierra el proceso Profesor

    for(int i = 0; i < N_EST; i++){                 // Cierra los 60 procesos Representantes

        R[i].join();

    }

    error = chan.Close();
    if(error == -1){
        
        cerr << chan.error("Error: Close",errno);

    }

    cout << "Prueba finalizada. " << endl;

    return 0;

}