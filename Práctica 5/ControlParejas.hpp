/*
 * Autores: Javier Julve Yubero, Alejandro Benedi Andres
 * NIP´s: 840710, 843826
*/


#include <iostream>
#include <mutex>
#include <condition_variable>

const int N_EST = 60; //# de estudiantes
const int N_FIL = N_EST /2; //# de filas en la matriz
const int N_COL = 1000; //# de columnas


using namespace std; 

class monitor{

    public:

        void constructor();
        void sillas_libres(int nip);                                
        void asignacion(int nip, int& devolver_fila, int& devolver_pareja);
        void fin_pareja(int nip, int& devolver_maximo);
        void terminado();
        void desbloquear_asignacion(int nip);
        void desbloquear_fin_pareja(int nip, int resultado);
        void desbloquear_terminado();
        void hay_estudiantes(int fila);


    private:

    condition_variable c_calculado[N_EST];
    condition_variable c_datos_pasado[N_EST];
    condition_variable c_sillas_menor_2;
    condition_variable c_sillas_igual_2;
    condition_variable c_terminado;

    mutex mtx_monitor;
    
    bool datos_pasados[N_EST];
    bool calculado[N_EST];

    int sillas_ocupadas;
    int contador;

    int NIPS[N_EST];
    int pareja[N_EST];
    int filas[N_EST];
    int maximo[N_EST];
        
};