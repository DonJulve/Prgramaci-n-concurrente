
//*****************************************************************
// File:   Servidor.cpp
// Author: PSCD-Unizar
// Date:   noviembre 2022
// Coms:   Ejemplo de servidor con comunicación síncrona mediante sockets
//         Compilar el fichero "Makefile" asociado, mediante
//         "make".
//         Invocación
//                  ./Servidor
//*****************************************************************

#include <iostream>
#include <set>     //manejo de conjuntos de las STL
#include "Socket.hpp"

using namespace std;

const int MESSAGE_SIZE = 4001; //mensajes de no más 4000 caracteres

//-------------------------------------------------------------
// Cuenta el número de vocales existentes en un mensaje
// PRE:
// POST: Devuelve el número de vocales existentes en el mensaje 'message'
int cuentaVocales(string message) {
    set<char> vocales = {'a','A','e','E','i','I','o','O','u','U'};
    int count = 0;

    for (int i=0; i < message.length(); i++) {
        if (vocales.find(message[i]) != vocales.end()) {
            count++;
        }
    }
    return count;
}
//-------------------------------------------------------------
int main(int argc,char* argv[]) {
    string MENS_FIN = "END OF SERVICE";
    // Puerto donde escucha el proceso servidor
    int SERVER_PORT = 3000;

    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(SERVER_PORT);


    // Bind
    int socket_fd =chan.Bind();
    if (socket_fd == -1) {
        cerr << chan.error("Error en el bind",errno);
        exit(1);
    }

    // Listen
    int error_code = chan.Listen();
    if (error_code == -1) {
        cerr << chan.error("Error en el lisen",errno);
        // Cerramos el socket
        chan.Close(socket_fd);
        exit(1);
    }

    // Accept
    int client_fd = chan.Accept();
    if (client_fd == -1) {
        cerr << chan.error("Error en el accept",errno);
        // Cerramos el socket
        chan.Close(socket_fd);
        exit(1);
    }

    // Buffer para recibir el mensaje
    int length = 100;
    string buffer;
    int rcv_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje

    bool out = false; // Inicialmente no salir del bucle
    while (!out) {
        // Recibimos el mensaje del cliente
        rcv_bytes = chan.Receive(client_fd, buffer, MESSAGE_SIZE);

        if(rcv_bytes == -1) {
            cerr << chan.error("Error al recibir datos",errno);
            // Cerramos los sockets
            chan.Close(client_fd);
            chan.Close(socket_fd);
        }

        cout << "Mensaje recibido: '" << buffer << "':" << rcv_bytes << endl;

        // Si recibimos "END OF SERVICE" --> Fin de la comunicación
        if (buffer == MENS_FIN) {
            out = true; // Salir del bucle
        } else {
            // Contamos las vocales recibidas en el mensaje anterior
            int num_vocales = cuentaVocales(buffer);

            // Enviamos la respuesta
            string resp = to_string(num_vocales);

            send_bytes = chan.Send(client_fd, resp);

            cout << "Mensaje enviado: '" << resp << "':" << send_bytes << endl;

            if(send_bytes == -1) {
                cerr << chan.error("Error al enviar datos",errno);
                // Cerramos los sockets
                chan.Close(client_fd);
                chan.Close(socket_fd);
                exit(1);
            }
        }
    }

    // Cerramos el socket del cliente
    cerr << "Cerrando cliente : " + to_string(client_fd) + "\n";
    error_code = chan.Close(client_fd);
    if (error_code == -1) {
        cerr << chan.error("Error cerrando el socket del cliente",errno);
    }

    // Cerramos el socket del servidor
    cerr << "Cerrando servidor : " + to_string(socket_fd) + "\n";
    error_code = chan.Close(socket_fd);
    if (error_code == -1) {
        cerr << chan.error("Error cerrando el socket del servidor",errno);
    }
    // Mensaje de despedida
    cout << "Bye bye" << endl;

    return error_code;
}
