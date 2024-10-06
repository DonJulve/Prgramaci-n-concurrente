//*****************************************************************
// File:   Semaphore_V4.hpp
// Author: PSCD-University of Zaragoza
// Date:   octubre 2016
// Coms:   Especificación de una clase de semáforos, extendida, con operaciones
//         "wait(nat)" y "signal(nat)". También se ha extendido con la posibilidad
//         de generar información de "logging"
//         El valor transmitido en la inicialización ha de ser
//         no negativo. Si fuera negativo, da error y aborta la ejecución.
//         La semántica que implementa es la que en
//         M. Ben-Ari, "Principles of Concurrent and Distributed Programming", Addison-Wesley, 2006
//         denomina "Busy-wait" (ver pág. 120)
//         Para los casos en que el uso del semáforo va a ser exclusivamente como "mutex", Cpp
//         suministra la clase "std::mutex", cuya especificación se encuentra en 
//         http://en.cppreference.com/w/cpp/thread/mutex
//
//         Versíón sin logging
//*****************************************************************

#ifndef SEMAPHORE_V4_HPP
#define SEMAPHORE_V4_HPP

#include <mutex>
#include <condition_variable>
#include <cassert>
#include <fstream>

//-----------------------------------------------------
using namespace std; //mutex, condition_variable, etc.

class Semaphore {
private:
    mutex mtx;                    //los dos primeros atributos se entenderán al estudiar los monitores
    condition_variable_any cv;
    volatile int count;                    //natural asociado al semáforo

public:
    //------------------------- constructores
    //Pre:  n>=0
    //Post: count=n
    //Coms: constructor con información para debugging
    //      Ejemplo: "Semaphore s(2);"
    Semaphore(const int n);

    //------------------------- destructor
    //Pre:  True
    //Post: El dato ha sido destruído
    //Coms: se invoca de manera automática al cerrarse el bloque donde ha sido declarado
    ~Semaphore();

    //------------------------- operaciones estándar
    //Pre:  True
    //Post: <count++>
    void signal();

    //Pre:  True
    //Post: <await count>0 
    //          count-- 
    //      >
    void wait();

    //------------------------- operaciones extendidas
    //Pre:  n>0
    //Post: <count=count+n>
    void signal(int n);

    //Pre:  n>0 
    //Post: <await count>=n 
    //          count = count-n
    //      >
    void wait(const int n);
};

#endif 
