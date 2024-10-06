//******************************************************************
// File:   ServidorMulticliente.cpp
// Author: PSCD-Unizar
// Date:   Noviembre 2022
// Coms:   Ejemplo de servidor multicliente con comunicación síncrona mediante sockets
//         Invocación
//                  ./ServidorMulticliente
//*****************************************************************

#include "Socket.hpp"
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

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
// Espera "secs" segundos y se conecta. Usado para desbloquear un "accept"
// Y pone "fin" a true
void timeOut(int secs,bool& fin,int port) {
	Socket chan("localhost", port);
	this_thread::sleep_for(chrono::seconds(secs));
	fin = true;
	int sfd = chan.Connect(); //sacará al servidor del "Accept"
	chan.Close();
}
//-------------------------------------------------------------
void servCliente(Socket& chan, int client_fd) {
    string MENS_FIN = "END OF SERVICE";
    // Buffer para recibir el mensaje
    int length = 100;
    string buffer;


    bool out = false; // Inicialmente no salir del bucle
    while(!out) {
        // Recibimos el mensaje del cliente
        int rcv_bytes = chan.Receive(client_fd,buffer,length);

        if (rcv_bytes == -1) {
            cerr << chan.error("Error al recibir datos",errno);
            // Cerramos los sockets
            chan.Close(client_fd);
        }

        cout << "Mensaje recibido: " + buffer + "\n";

        // Si recibimos "END OF SERVICE" --> Fin de la comunicación
        if (buffer == MENS_FIN) {
            out = true; // Salir del bucle
        } else {
            // Contamos las vocales recibidas en el mensaje anterior
            int num_vocales = cuentaVocales(buffer);

            // Enviamos la respuesta
            string s = to_string(num_vocales);

            int send_bytes = chan.Send(client_fd, s);
            if(send_bytes == -1) {
                cerr << chan.error("Error al enviar datos",errno);
                // Cerramos los sockets
                chan.Close(client_fd);
                exit(1);
            }
        }
    }
    chan.Close(client_fd);
}
//-------------------------------------------------------------
int main(int argc,char* argv[]) {
    const int N = 4;
    // Puerto donde escucha el proceso servidor
    int SERVER_PORT = 3000; //normalmente será un parámetro de invocación. P.e.: argv[1]
    vector<thread> cliente;
    vector<int> client_fds;

    bool fin = false;

    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(SERVER_PORT);

    // bind
    int socket_fd = chan.Bind();
    if (socket_fd == -1) {
        cerr << chan.error("Error en el bind",errno);
        exit(1);
    }

    //lisen
    int error_code = chan.Listen();
    if (error_code == -1) {
        cerr << chan.error("Error en el listen",errno);
        // Cerramos el socket
        chan.Close();
        exit(1);
    }
    //para desbloquear servidor y terminar
    thread timeControl(&timeOut,60,ref(fin),SERVER_PORT);

    int i = 0;
    while (!fin) {
    	i++;
        // Accept
        int new_client_fds = chan.Accept();

        if(new_client_fds == -1) {
            cerr << chan.error("Error en el accept",errno);
            // Cerramos el socket
            chan.Close();
            exit(1);
        } else {
        	if (!fin) {
                //introducir en el vector el cliente y arrancar el thread
        	    client_fds.push_back(new_client_fds);
        	    cliente.push_back(thread(&servCliente, ref(chan), new_client_fds));
        	    cout << "Nuevo cliente " + to_string(i) + " aceptado" + "\n";
        	}
        	else {
        		cout << "He acabado" << endl;
        	}
        }
    }

    //¿Qué pasa si algún thread acaba inesperadamente?
    for (int i=0; i<cliente.size(); i++) {
        cliente[i].join();
    }
    timeControl.join();

    // Cerramos el socket del servidor
    error_code = chan.Close();
    if (error_code == -1) {
        cerr << chan.error("Error cerrando el socket del servidor",errno);
    }

    // Despedida
    cout << "Bye bye" << endl;

    return error_code;
}
//-------------------------------------------------------------
