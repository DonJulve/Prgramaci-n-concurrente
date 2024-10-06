//Autores: Javier Julve     NIP:840710
//Autores: Alejandro Benedi     NIP:843826

#include <iostream>
#include <string>
#include <thread>
#include <iomanip>
#include "librerias/Semaphore_V4/Semaphore_V4.hpp"

using namespace std;

const int N_EST = 60; //# de estudiantes
const int N_FIL = N_EST /2; //# de filas en la matriz
const int N_COL = 1000; //# de columnas

struct Estudiante{      

    Semaphore* b_datosPasado = new Semaphore(0);
    int d_datosPasado = 0;

    Semaphore* b_calculado = new Semaphore(0);
    int d_calculado = 0;

    int resultado = 0;

    int fila = 0;

    int pareja = 0;

    bool datos_pasado = false;

    bool calculado = false;
};

struct Profesor{        

    Semaphore* b_sillasMenor2 = new Semaphore(0);
    int d_sillasMenor2 = 0;

    Semaphore* b_sillasIgual2 = new Semaphore(0);
    int d_sillasIgual2 = 0;

    Semaphore* b_terminado = new Semaphore(0);
    int  d_terminado = 0;

    int sillas_ocupadas = 0;

    int contador = 0;
};



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
void mostrarResultado(struct Estudiante* estudiantes[], struct Profesor& profesor, const int nip, const int suma){

    cout << to_string(estudiantes[nip]->fila) + "   |  " + to_string(nip) + "-" + to_string(estudiantes[nip]->pareja) + "  |  " 
        + to_string(estudiantes[estudiantes[nip]->pareja]->resultado) + "  |  " + to_string(suma) + "\n";
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




//  PASAR TESTIGO
//  Esta funcion se encarga de desbloquear los procesos de una forma ordenada y controlada para que 
//  se pueda ejecutar de forma correcta todo el programa.
void pasarTestigo(Semaphore& testigo, struct Estudiante* estudiantes[], struct Profesor& profesor) {

    bool testigo_pasado = false;
    int i = 0;

    while(i<N_EST && !testigo_pasado){

        if(estudiantes[i]->datos_pasado && estudiantes[i]->d_datosPasado > 0){
            estudiantes[i]->d_datosPasado--;
            estudiantes[i]->b_datosPasado->signal();
            testigo_pasado = true;
        }
        else if(estudiantes[estudiantes[i]->pareja]->calculado && estudiantes[i]->d_calculado > 0){
            estudiantes[i]->d_calculado--;
            estudiantes[i]->b_calculado->signal();
            testigo_pasado = true;
        }
        
        i++;
    }
    
    if(profesor.sillas_ocupadas < 2 && profesor.d_sillasMenor2 > 0 && !testigo_pasado){
        profesor.d_sillasMenor2--;
        profesor.b_sillasMenor2->signal();
    }
    else if(profesor.sillas_ocupadas >= 2 && profesor.d_sillasIgual2 > 0 && !testigo_pasado){
        profesor.d_sillasIgual2--;
        profesor.b_sillasIgual2->signal();
    }
    else if(profesor.contador == N_EST/2 && profesor.d_terminado > 0 && !testigo_pasado){
        profesor.d_terminado--;
        profesor.b_terminado->signal();
    }
    else if(!testigo_pasado){
        testigo.signal();
    }
}



//  PROCESO ESTUDIANTE
//  El proceso espera bloqueado si las sillas estan ocupdas.Una vez que esten libres y haya 2 estudiantes sentados el profesor les dejara entrar y le dira a cada miembro de la pareja 
//  el nip del otro miembro y la fila.Tras ello el el miembro con nip mas bajo se encargara de buscar el maximo 
//  de la fila.El otro miembrosera el encargado de hacer la suma de la fila que les ha tocado, de mostrar los 
//  resultados obtenidos por ambos miembros y de comunicar al profesor que la paraja ha terminado.
void Estudiante ( int nip, int D[][N_COL], Semaphore& testigo, struct Estudiante* estudiantes[], struct Profesor& profesor,
                    int& NIP1, int& NIP2) {
        
    testigo.wait();
    if(!(profesor.sillas_ocupadas < 2)){          
        profesor.d_sillasMenor2++;
        testigo.signal();
        profesor.b_sillasMenor2->wait();                // espera hasta que haya sitio para sentarse
    }

    profesor.sillas_ocupadas++;

    if(profesor.sillas_ocupadas == 1){
        NIP1 = nip;
    }
    else{
        NIP2 = nip;
    }

    pasarTestigo(testigo, estudiantes, profesor);


    testigo.wait();
    if(!(estudiantes[nip]->datos_pasado)){
        estudiantes[nip]->d_datosPasado++;
        testigo.signal();
        estudiantes[nip]->b_datosPasado->wait();        // espera hasta que el profesor le diga la fila y el nip de su compañero
    }
    pasarTestigo(testigo, estudiantes, profesor);

    if (nip < estudiantes[nip]->pareja) {
        estudiantes[nip]->resultado = maxFila(D,estudiantes[nip]->fila);
        testigo.wait();
        estudiantes[nip]->calculado = true;             // le comunica al otro miembro de la pareja que ha termiando de calcular
        pasarTestigo(testigo, estudiantes, profesor);               

    }
    else {
        
        int suma = sumaFila(D,estudiantes[nip]->fila);
        
        //Esperar al resultado del otro miembro de la pareja
        testigo.wait();
        if(!estudiantes[estudiantes[nip]->pareja]->calculado){
            estudiantes[nip]->d_calculado++;
            testigo.signal();
            estudiantes[nip]->b_calculado->wait();       // Espera hasta que su compañero termine
        }
        
        pasarTestigo(testigo, estudiantes, profesor);

        mostrarResultado(estudiantes, profesor, nip, suma);     //Muestra en pantalla el resultado

        testigo.wait();
        profesor.contador++;                                   // Comunica al profesor que la pareja ha termiando
        pasarTestigo(testigo, estudiantes, profesor);
    }
}



//  PROCESO PROFESOR
//  Inicialmente este proceso se mantendra a la espera hasta que se siente 2 alumnos.Tras ello el proceso
//  profesor comunicara a cada miembro de la pareja la fila que les ha tocado y el nip del otro miebro y 
//  les dejara entrar ha realizar la prueba.Tras formar las parejas el profesor se quedara bloqueado
//  hasta que todas las parejas hayan terminado la prueba.
void Profesor (Semaphore& testigo, struct Estudiante* estudiantes[], struct Profesor& profesor,
                    int& NIP1, int& NIP2){

    for( int i=0; i< N_FIL ; i++) {
        
        testigo.wait();             
        if(!(profesor.sillas_ocupadas >= 2)){
            profesor.d_sillasIgual2++;
            testigo.signal();
            profesor.b_sillasIgual2->wait();                        // espera bloqueado hasta que haya 2 estudiantes
        }

        estudiantes[NIP1]->fila = i;
        estudiantes[NIP2]->fila = i;
        estudiantes[NIP1]->pareja = NIP2;
        estudiantes[NIP2]->pareja = NIP1;
        profesor.sillas_ocupadas = 0;
        estudiantes[NIP1]->datos_pasado = true;                     
        estudiantes[NIP2]->datos_pasado = true;

        pasarTestigo(testigo, estudiantes, profesor);
        
    }

    // esperar que todos hayan terminado
    testigo.wait();
        if(!(profesor.contador == N_EST/2)){
            profesor.d_terminado++;
            testigo.signal();
            profesor.b_terminado->wait();
        }
    pasarTestigo(testigo, estudiantes, profesor);
}



int main () {

    int D[ N_FIL ][ N_COL ]; // para almacenar los datos

    Semaphore testigo(1);
    struct Estudiante* estudiantes[N_EST];           // vector de structuras que tienen todos los tipos de semaforoas para poder controlar cada proceso estudiante
    
    for (int i = 0; i < N_EST; i++){                 
      estudiantes[i] = new struct Estudiante();     
    }
    
    struct Profesor profesor;                       // estructura con todo los tipos de semaforos para controlar el proceso profesor

    int NIP1 = -1, NIP2 = -1;

    cargar(D);                                      // Carga los datos en la matriz D   

    thread P;
    thread E[N_EST];
    
    P = thread(&Profesor,ref(testigo),estudiantes,ref(profesor),ref(NIP1),ref(NIP2));                   // "Crea" el proceso profesor
 
    for (int i = 0; i < N_EST; i++){
        E[i] = thread(&Estudiante, i, D, ref(testigo),estudiantes,ref(profesor),ref(NIP1),ref(NIP2));   // "Crea" los 60 procesos estudiantes
    }

    P.join();                                                                                           // "Cierra" el proceso profesor

    for (int i = 0; i < N_EST; i++){
        E[i].join();                                                                                    // "Cierra" los 60 procesos estudiantes
        delete estudiantes[i];
    }


    cout << " Prueba finalizada \n";
    return 0;
}


