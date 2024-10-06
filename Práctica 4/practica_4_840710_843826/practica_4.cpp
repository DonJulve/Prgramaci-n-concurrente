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
#include "ControlParejas.hpp"

using namespace std;


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


//  Pre: ---
//  Post: Muestra el resultado en forma de tabla 
void mostrarResultado(int fila[], int nip, int pareja[], int maximo, int suma){


    cout << to_string(fila[nip]) + "   |  " + to_string(pareja[nip]) + "-" + to_string(nip) + "  |  " 
        + to_string(maximo) + "  |  " + to_string(suma) + "\n";
}


//  PRE: ---
//  POST: Introduce en la matriz <D> los datos del fichero de texto "datos.txt"
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





//  PROCESO ESTUDIANTE
//  El proceso espera bloqueado si las sillas estan ocupdas.Una vez que esten libres y haya 2 estudiantes sentados el profesor 
//  les dejara entrar y le dira a cada miembro de la pareja el nip del otro miembro y la fila.Tras ello el el miembro 
//  con nip mas bajo se encargara de buscar el maximo de la fila.El otro miembro sera el encargado de hacer la suma 
//  de la fila que les ha tocado, de mostrar los resultados obtenidos por ambos miembros y de comunicar al profesor que la 
//  pareja ha terminado.
void Estudiante ( int nip, int D[][N_COL], int pareja[], monitor& mon, int maximo[], int NIPS[], int fila[]) {

    int retardo = (0 + rand() % (5 - 0)) * 1000;
    this_thread::sleep_for(chrono::milliseconds(retardo));                  // "Duerme un numero aleatorio de milisegundos"

    mon.sillas_libres(nip, NIPS);

    mon.asignacion(nip);

    if (nip < pareja[nip]) {

        maximo[fila[nip]] = maxFila(D, fila[nip]);

        int retardo = (0 + rand() % (5 - 0)) * 1000;
        this_thread::sleep_for(chrono::milliseconds(retardo));              // "Duerme un numero aleatorio de milisegundos"

        mon.desbloquear_fin_pareja(nip, pareja);              

    }
    else {
        
        int suma = sumaFila(D, fila[nip]);
        
        mon.fin_pareja(nip, pareja);

        mostrarResultado(fila, nip, pareja, maximo[fila[nip]], suma);       //Muestra en pantalla el resultado

        mon.desbloquear_terminado();
    }
}


//  PROCESO PROFESOR
//  Inicialmente este proceso se mantendra a la espera hasta que se siente 2 alumnos.Tras ello el proceso
//  profesor comunicara a cada miembro de la pareja la fila que les ha tocado y el nip del otro miembro y 
//  les dejara entrar ha realizar la prueba.Tras formar las parejas el profesor se quedara bloqueado
//  hasta que todas las parejas hayan terminado la prueba.
void Profesor (monitor& mon, int pareja[], int NIPS[], int filas[]){

    for( int i=0; i< N_FIL ; i++) {
        
        mon.hay_estudiantes(pareja, NIPS, filas, i);
        
    }

    // esperar que todos hayan terminado
    mon.terminado();
}



int main(){
    
    srand(time(nullptr));               // Crea la semilla

    int D[N_FIL][N_COL];

    int NIPS[N_EST];
    int pareja[N_EST];
    int filas[N_EST];
    int maximo[N_EST];

    thread P;
    thread E[N_EST];

    monitor mon;

    cargar(D);

    P = thread(&Profesor, ref(mon), pareja, NIPS, filas);

    for(int i = 0; i < N_EST; i++){
        E[i] = thread(&Estudiante, i, D, pareja, ref(mon), maximo, NIPS, filas);
    }

    P.join();

    for(int i = 0; i < N_EST; i++){
        E[i].join();
    }

    cout << "\nPrueba finalizada \n";
    return 0;
    
}

