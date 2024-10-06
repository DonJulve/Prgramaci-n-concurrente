//------------------------------------------------------------------------------
// File:   main_p2_e1.cpp
// Author: Javier Julve, Alejandro Benedí
// Date:   octubre 2022
// Coms:   2º practica PSCD
//------------------------------------------------------------------------------
#include <iostream>
#include <thread>
#include <fstream>

using namespace std;


const int N = 512;
const int N_BUSC = 8; //# de procesos buscadores
using VectInt = int[N]; //"VectInt" es un "alias" para vector de int de dim. N
const int numPos = N/N_BUSC;  // 512 / 8 = 64

bool terminado[N_BUSC];
bool datosCargados = false;

/*
 *Pre: El coordinador recibe por referencia un vector de N dimensiones en el que almacenara datos de un fichero previamente declarado.
 *
 *Post: El coordinador abre el fichero y transfiere todos los datos del mismo a un vector, posteriormente confirma su lectura, para que
 *una vez que el buscador haya hecho su trabajo, el coordinador contara cuantas veces se ha repetido el numero seleccionado en los distintos
 *procesos.
*/
void coordinador(VectInt v, int resultado[]){
    
    ifstream f;
    f.open("datos.txt");
    if (f.is_open()){
        int dato_n;
        int i=0;
        while(!f.eof()){
            f>>dato_n;
            v[i]=dato_n;    //transferencia de datos al vector
            i++;
        }
        f.close();
    }
    else{
        cerr<<"No se encontro el archivo de datos.txt";
    }

    datosCargados = true;   //confirmacion de carga

    for(int i = 0; i < N_BUSC; i++){
        while(!terminado[i]){   //confirmacion de que el buscador ha acabado los procesos
        }
    }

    int sumaTotal = 0;
    for(int i = 0; i < N_BUSC; i++){
        sumaTotal = sumaTotal + resultado[i];   //cuenta de veces repetido
    }

    cout << "\nEl numero de veces que se repite ese numero es: " << sumaTotal << endl; 
}


/*
 *Pre: El buscador recibe del coordinador la confirmacion de que los datos han sido cargados con exito y transferidos al
 *vector v, ademas la i marca la casilla inicial de cada proceso y la d es la final, value es el valor a buscar y result una variable
 *en la que vamos a ir contando la cantidad de veces que aparece el valor en un solo proceso.
 *
 *Post: La funcion almacena en result la cantidad de veces que aparece un dato en un proceso y a su vez se pasa por referencia a una
 *casilla del vector de resultados para poder contar todo despues, una vez que un proceso termina, su casilla correspondiente en el vector
 *de booleanos (confirmador para el coordinador) es puesto en true.
*/

 void search(const VectInt v, const int i, const int d, const int value, int& result){
    
    result = 0;

    while(!datosCargados){} //Recibe del coordinador la informacion de que los datos se han cargado con exito.
    
    for (int j = i; j <= d; j++){

        if (v[j] == value){
            result++;   //Aumenta en 1 en caso de que exista el valor en el proceso
        }
    }

    terminado[i/numPos] = true; //Pone todos los procesos a true eventualmente para avisar al coordinador que ya terminó.
}


int main(){

    VectInt v;
    int valor = 0;
    int resultado[N_BUSC];

    cout << "Que valor quiere buscar: ";
    cin >> valor;
    
    thread C=thread(&coordinador, v, resultado);

    thread B[N_BUSC];
    
    for (int j = 0; j < N_BUSC; j++){

        int i = j * numPos;
        int d = (j+1) * numPos - 1;

        B[j] = thread(&search, v, i, d, valor, ref(resultado[j]));
    }

    C.join();

    for (int i = 0; i < N_BUSC; i++){
        B[i].join();
    }
}
