//*****************************************************************
// File:   Socket.hpp
// Author: PSCD-Unizar
// Date:   noviembre 2015
// Coms:   Especificación de una librería genérica para 
//         comunicación síncrona utilizando sockets
//*****************************************************************

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <string>
#include <set>

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0) //hendrix lo necesita

using namespace std;

class Socket {
private:
	// ATRIBUTOS
	// Para almacenar la dirección y número de puerto
	// donde escucha el proceso servidor
	string SERVER_ADDRESS;
	int SERVER_PORT;

	// Descriptor de fichero asociado al socket;
	int socket_fd;

	// Conjunto de "file descriptors" creados por un servidor
	// Uno por cada "accept" a un cliente
	set<int> fileDescClients;


public:
	// CONSTRUCTORES
	// Constructor de la clase indicando la dirección
	// y el puerto de escucha del servidor. Para clientes.
	Socket(string address, int port);


	// Constructor de la clase indicando  el puerto de 
	// escucha del servidor. Para servidores.
	Socket(int port);

	// Cierra todos los sockets asociados
	~Socket();

	// MÉTODOS

	// Acepta la conexión de un nuevo cliente.
	// PRE:
	// POST: Abre una nueva conexión con un cliente.
	// Devuelve el descriptor de fichero del socket asociado
	// al nuevo cliente en caso de exito.
	// Devuelve -1 en caso de error.
	int Accept();

	// Binding de la nueva conexión.
	// PRE: 
	// POST: Asocia la nueva conexión con el programa actual.
	// Devuelve el descriptor de fichero del socket asociado
	// a la conexión en caso de éxito. 
	// Devuelve -1 en caso de error.
	int Bind();

	// Cierre de conexión.
	// PRE: 'fd' indica un descriptor de fichero asociado
	//      a un socket con una conexión abierta.
	// POST: Cierra la conexión asociada al descriptor de
	//       fichero indicado.
	// Devuelve 0 en caso de éxito.
	// Devuelve -1 en caso de error. 
	// Com:  para usar desde el lado del servidor y cerrar sockets abiertos
	//       para atender a clientes
	int Close(int fd);

	// Cierra el socket
	int Close();


	// Conexión con el servidor.
	// PRE: 
	// POST: La función conecta con el servidor utilizando
	//       el socket especificado. 
	// En caso de éxito el método devuelve el descriptor
	// de fichero del socket 
	// a utilizar para enviar y recibir mensages.
	// Devuelve -1 en caso de error. 
	int Connect();

	// Llamada a listen.
	// PRE: 
	// POST: El servidor comienza a escuchar en el
	//       puerto indicado.
	// Devuelve 0 en caso de éxito.
	// Devuelve -1 en caso de error.
	int Listen();

	// Recepción de un nuevo mensaje a través del 
	// socket abierto.
	// PRE: 'fd' indica un descriptor de fichero asociado
	//       a un socket con una conexión abierta.
	//       buffer: Puntero al buffer donde se almacenaran 
	//       los datos leídos.
	//       buffer_length indica el tamaño del buffer.
	// POST: Recibe un nuevo mensaje que es almacenado 
	//       en la variable "buffer".
	// Devuelve el numero de bytes leídos en caso de éxito.
	// Devuelve -1 en caso de error.
	int Receive(int fd, char* buffer, int buffer_length);
	int Receive(int fd, string &buffer, int buffer_length);

	// Envía información a través del socket abierto
	// PRE: 'fd' indica un descriptor de fichero asociado 
	//       a un socket con una conexión abierta.
	//       "message" ha de ser NO VACÍO
	// POST: Envía el mensaje establecido a través del 
	//       socket indicado
	// Devuelve el número de bytes enviados en caso de éxito.
	// Devuelve -1 en caso de error.
	ssize_t Send(int fd, const char* message);
	ssize_t Send(int fd, const string message);

	// Genera un mensaje de error
	// PRE: 'cuerpoMens' es el texto principal del mensaje
	//      'errNum' es el código de error según
	// POST: Recibe un nuevo mensaje que es almacenado 
	//       en la variable "buffer".
	string error(string cuerpoMens, int errNum);
};

#endif
