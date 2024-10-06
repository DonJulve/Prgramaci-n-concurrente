//------------------------------------------------------------------------------
// File:   main_p2_e2.cpp
// Author: Javier Julve, Alejandro Benedí
// Date:   octubre 2022
// Coms:   2º practica PSCD
//------------------------------------------------------------------------------
#include <iostream>
#include <thread>
#include <fstream>
#include <atomic>

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
 *procesos y devolvera el comienzo y final del trozo donde más se repite, así como la cantidad de veces que se repite en el trozo.
*/
void coordinador(VectInt v, int& maxVeces, int& indMin, int& indMax){
    
    ifstream f;
    f.open("datos.txt");
    if (f.is_open()){
        int dato_n;
        int i=0;
        while(f>>dato_n){
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
        while(!terminado[i]){}   //confirmacion de que el buscador ha acabado los procesos
    }

    cout << "\nLa mayor cantidad de veces que se repite un numero en un trozo de vector es: " << maxVeces << endl;  //Se indican las veces que se repite un numero en un trozo
    cout << "El indice del inicio del vector es: " <<  indMin << endl;  //Se indica el comienzo del trozo donde ocurre lo anterior
    cout << "El indice del final del vector es: " << indMax << endl;    //Se indica el final del trozo donde ocuerre lo anterior
}


/*
 *Pre: El buscador recibe del coordinador la confirmacion de que los datos han sido cargados con exito y transferidos al
 *vector v, ademas la i marca la casilla inicial de cada proceso y la d es la final, value es el valor a buscar y maxVeces una variable
 *en la que vamos a ir contando la cantidad de veces que aparece el valor en un solo proceso (y que solo se actualizará si es el mayor número hasta ahora),
 *además indMin e indMax corresponde al inicio y final del trozo de vector donde mas se ha repetido un valor.
 *
 *Post: La funcion almacena en maxVeces la cantidad de veces que aparece un dato en un proceso (siempre que sea el mayor número de aparición de dicho dato en 
 *cualquier proceso), una vez confirmado esto, se devolverán los índices de inicio y fin del trozo para poder seguirlos comparando en un futuro
 *y que haya una posible actualización. Cuando un proceso termina, este se pone a true en su respectiva posición del vector de booleanos
 *y solo cuando todas las casillas sean true el buscador habrá acabado y le informará al coordinador de ello.
*/

void search(const VectInt v, const int i, const int d, const int value, int& maxVeces, int& indMin, int& indMax, atomic_flag& tas){
    
    int veces = 0;

    while(!datosCargados){} //Recibe del coordinador la informacion de que los datos se han cargado con exito.
    
    for (int j = i; j <= d; j++){

        if (v[j] == value){
            veces++;   //Aumenta en 1 en caso de que exista el valor en el proceso
        }

    }

    while(tas.test_and_set()){}

    if(veces > maxVeces){
        maxVeces = veces;   //Número más alto de veces que ha aparecido el valor en un trozo
        indMin = i;     //Indice de comienzo del trozo
        indMax = d;     //Indice de final del trozo
    }

    tas.clear();

    terminado[i/numPos] = true; //Pone todos los procesos a true eventualmente para avisar al coordinador que ya terminó.
}

int main(){

    VectInt v;
    int valor = 0, maxVeces = 0, indMin = 0, indMax = 0;
    atomic_flag tas = ATOMIC_FLAG_INIT;

    cout << "Que valor quiere buscar: ";
    cin >> valor;
    
    thread C=thread(&coordinador, v, ref(maxVeces), ref(indMin), ref(indMax));

    thread B[N_BUSC];
    
    for (int j = 0; j < N_BUSC; j++){

        int i = j * numPos;
        int d = (j+1) * numPos - 1;

        B[j] = thread(&search, v, i, d, valor, ref(maxVeces), ref(indMin), ref(indMax), ref(tas));
    }

    C.join();

    for (int i = 0; i < N_BUSC; i++){
        B[i].join();
    }
}
