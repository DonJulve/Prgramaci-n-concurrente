//*****************************************************************
// File:   Socket.cpp
// Author: PSCD-Unizar
// Date:   noviembre 2022
// Coms:   Implementación de una librería genérica para
//         comunicación síncrona utilizando sockets
//         La especificación se encuentra en "Socket.hpp"
//         Como realiza "#include <Socket.hpp>", es necesario que
//         se indique al compilador que debe buscar ficheros
//         para incluir también en el directorio donde se encuentre
//         "Socket.hpp", mediante
//              -IpathDondeSeEncuentaSocket.hpp
//*****************************************************************

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <Socket.hpp>

using namespace std;

//-------------------------------------------------------------
const int ACK_BUFFER_SIZE = 4; //para hacer la comunición síncrona
//-------------------------------------------------------------
// Constructor de la clase indicando la dirección
// y el puerto de escucha del servidor. Para clientes.
Socket::Socket(string address, int port) {

    // Guardamos datos del servidor
    SERVER_ADDRESS = address;
    SERVER_PORT = port;
}
//-------------------------------------------------------------
// Constructor de la clase indicando solo el puerto de
// escucha del servidor. Para servidores.
Socket::Socket(int port) {

    // Guardamos datos del servidor
    SERVER_ADDRESS = "localhost"; //127.0.0.1
    SERVER_PORT = port;
}
//-------------------------------------------------------------
// Destructor. Cerrar todo lo que quede
Socket::~Socket() {
    for (auto itr = fileDescClients.begin(); itr != fileDescClients.end(); itr++) {
        close(*itr);
    }
}
//-------------------------------------------------------------
int Socket::Accept() {

    struct sockaddr_in client;  // Información dirección scliente
    socklen_t sin_size=sizeof(struct sockaddr_in);
    int fd = accept(socket_fd,(struct sockaddr *) &client, &sin_size);

    fileDescClients.insert(fd);

    if(fd==-1) {
        cerr << "Error en accept\n";
    }
    return fd;
}
//-------------------------------------------------------------.
int Socket::Bind() {

    // Creación del socket y almacenamiento del descriptor del socket (Servidor)
    // AF_INET     --> IPv4
    // SOCK_STREAM --> Comunicación TCP
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Información de la dirección del servidor
    struct sockaddr_in server;

    server.sin_family = AF_INET; // IPv4
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY coloca nuestra dirección IP automáticamente
    bzero(&(server.sin_zero),8); // 0 en el resto de la estructura

    // Llamada a bind
    int exito = bind(socket_fd, (struct sockaddr *) &server, sizeof(struct sockaddr_in));

    if(exito == -1) {
        return -1;
    } else {
        fileDescClients.insert(socket_fd);
        return socket_fd;
    }
}
//-------------------------------------------------------------
int Socket::Close(int fd) {
    int exito;
    auto pos = fileDescClients.find(fd);
    
    if(pos == fileDescClients.end()) { //no es uno de los míos
        cerr << "No puedo cerrar un socket que no haya creado yo mismo\n";
        exito = -1;
    }
    else {
        fileDescClients.erase(pos);
        exito = close(fd);
        if(exito != 0) {
            exito -1;
        }
    }
    return exito;
}
//-------------------------------------------------------------
int Socket::Close() {

    int exito = close(socket_fd);

    if(exito != 0) {
        return -1;
    }

    return exito;
}
//-------------------------------------------------------------
int Socket::Connect() {

    // Creación del socket y almacenamiento del descriptor del socket (Cliente)
    // AF_INET     --> IPv4
    // SOCK_STREAM --> Comunicación TCP
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct hostent *he; // Información del nodo remoto
    struct sockaddr_in server;  // Información dirección servidor

    // Obtenemos la dirección del servidor
    he = gethostbyname(SERVER_ADDRESS.c_str());
    if (he==NULL) {
        cerr << "Error obteniendo la dirección del servidor\n";
        return -1;
    }

    // Guardamos la información del servidor
    server.sin_family = AF_INET;	// IPv4
    server.sin_port = htons(SERVER_PORT); // Codificación por red
    server.sin_addr = *((struct in_addr *)he->h_addr); // Info del servidor
    bzero(&(server.sin_zero),8); // Rellenamos con ceros

    // Establecemos la conexion con el servidor
    int exito = connect(socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr));
    if(exito==-1) {
        cerr << "Error conectando con el servidor " + SERVER_ADDRESS + ": " + to_string(SERVER_PORT) + "\n";
        return -1;
    } else {
        return socket_fd;
    }
}
//-------------------------------------------------------------
// Listen. Acerca de "max_connections"
// El serividor se pone a la escucha. "max_conexions_espera" define el
// tamaño de la cola con las conexiones pendiente. Una vez llena
// si acude un nuevo proceso, recibirá el error ECONNREFUSED avisándole de que
// no se le ha puesto a la cola. Deberé reintentarlo
int Socket::Listen() {
    const int max_conexions_espera = 10;
    // Llamamos a listen
    int exito = listen(socket_fd, max_conexions_espera);

    return exito;
}
//-------------------------------------------------------------
int Socket::Receive(int fd, char* buffer, int buffer_length) {

    // PRIMERO: RECIBIMOS INFORMACION
    // Limpiamos el buffer
    bzero(buffer, buffer_length);

    // Leemos todos los datos posibles que quepan en el buffer
    int num_bytes = recv(fd, buffer, buffer_length, 0);

    if (num_bytes == -1) {
        cerr << "Error al recibir datos del socket\n";
    }


    // SEGUNDO: ENVIAMOS ACK

    char ack[ACK_BUFFER_SIZE];

    bzero(ack, ACK_BUFFER_SIZE);
    strcpy(ack, "ACK");

    int sent_bytes = send(fd, ack, strlen(ack), 0);

    // Devolvemos número de bytes leídos
    return num_bytes;
}
//-------------------------------------------------------------
int Socket::Receive(int fd, string &buffer, int buffer_length) {
    buffer = ""; //vaciar
    // PRIMERO: RECIBIMOS INFORMACION
    // Limpiamos el buffer
    char bufferAux[buffer_length];
    bzero(bufferAux, buffer_length);

    // Leemos todos los datos posibles que quepan en el buffer
    int num_bytes = Receive(fd, bufferAux, buffer_length);

    if (num_bytes != -1) { //ha ido bien
        buffer.append(bufferAux);
    }

    // Devolvemos número de bytes leídos
    return num_bytes;
}
//-------------------------------------------------------------
ssize_t Socket::Send(int fd, const char* message) {

    // PRIMERO ENVIAMOS INFORMACION
    ssize_t num_bytes = send(fd, message, strlen(message), 0);

    // SEGUNDO: RECIBIMOS ACK
    // Creamos buffer para ACK
    char ack_buffer[ACK_BUFFER_SIZE];
    bzero(ack_buffer, ACK_BUFFER_SIZE);

    // Leemos todos los datos posibles que quepan en el buffer
    //Será "ACK"
    int rcv_bytes = recv(fd, ack_buffer, ACK_BUFFER_SIZE-1, 0);

    // Comprobamos que no haya error enviando el ACK
    if( 0 != strcmp(ack_buffer,"ACK") ) {
        num_bytes = -1;
    }

    return num_bytes;
}
//-------------------------------------------------------------
ssize_t Socket::Send(int fd, const string message) {

    ssize_t num_bytes = Send(fd, message.c_str());

    return num_bytes;
}
//-------------------------------------------------------------
string Socket::error(string cuerpoMens, int errNum) {
    string errString = strerror(errNum);
    return cuerpoMens + ": " + errString + "\n";
}
