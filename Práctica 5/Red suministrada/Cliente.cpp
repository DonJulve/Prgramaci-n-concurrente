//*****************************************************************
// File:   Cliente.cpp
// Author: PSCD-Unizar
// Date:   noviembre 2022
// Coms:   Ejemplo de cliente con comunicación síncrona mediante sockets
//         Invocación
//                  ./Cliente
//*****************************************************************
#include <iostream>
#include <chrono>
#include <thread>
#include "Socket.hpp"

using namespace std;

const int MESSAGE_SIZE = 4001; //mensajes de no más 4000 caracteres

int main(int argc,char* argv[]) {
    const string MENS_FIN = "END OF SERVICE";
    // Dirección y número donde escucha el proceso servidor
    string SERVER_ADDRESS = "localhost"; //alternativamente "127.0.0.1"
    int SERVER_PORT = 3000;

    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(SERVER_ADDRESS, SERVER_PORT);

    // Conectamos con el servidor. Probamos varias conexiones
    const int MAX_ATTEMPS = 10;
    int count = 0;
    int socket_fd;
    do {
        // Conexión con el servidor
        socket_fd = chan.Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socket_fd == -1) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while(socket_fd == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if(socket_fd == -1) {
        return socket_fd;
    }

    string mensaje;
    int read_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje

    do {
        // Leer mensaje de la entrada estandar
        do {
            cout << "Frase para contar las vocales: ";
            getline(cin, mensaje);
        } while (mensaje == ""); //no se puede enviar mensaje vacío
        
        // Enviamos el mensaje
        send_bytes = chan.Send(socket_fd, mensaje);

        if(send_bytes == -1) {
            cerr << chan.error("Error al enviar datos",errno);
            // Cerramos el socket
            chan.Close();
            exit(1);
        }

        if(mensaje != MENS_FIN) {
            // Buffer para almacenar la respuesta
            string buffer;

            // Recibimos la respuesta del servidor
            read_bytes = chan.Receive(socket_fd, buffer, MESSAGE_SIZE);

            // Mostramos la respuesta
            cout << "Mensaje enviado: '" << mensaje << "'" << endl;
            cout << "Numero de vocales: " << buffer << endl;
        }
    } while(mensaje != MENS_FIN);

    // Cerramos el socket
    int error_code = chan.Close();
    if(error_code == -1) {
        cerr << chan.error("Error cerrando el socket",errno);
    }

    // Despedida
    cout << "Bye bye" << endl;

    return error_code;
}
