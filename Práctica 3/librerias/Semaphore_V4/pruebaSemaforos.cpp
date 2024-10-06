//*****************************************************************
// File:   pruebaSemaforos.cpp
// Author: PSCD-Unizar
// Date:   octubre 2019
// Coms:   Ejemplo de uso de la clase "Semaphore", para la práctica 3
//         Compilar mediante el "Makefile" asociado
//*****************************************************************

#include <iostream>
#include <thread>
#include <string>
#include <cstdlib>

#include <Semaphore_V4.hpp>

using namespace std;

//-----------------------------------------------------
//  Compilación condicional: ¿quiero hacer "logging"?
#ifdef LOGGING_MODE
    #include <Logger.hpp>
    Logger _logger("_log_.log", 1024);
    //Logger _logger("_log_2.log", cout, 1); // si queremos echo de eventos en stdout
    string nada("");
    #define ADD_EVENT(e) {_logger.addMessage(nada+e);} //generar evento
#else
    #define ADD_EVENT(e)   // nada
#endif

//----------------------------------------------------
//Pre:
//Post: irrelevante
void func1(int n, Semaphore& s1, Semaphore& s2) {

    for(int i=0; i<n; i++) {
        s1.wait();
        cout << "ping\n";
        s2.signal();
    }
}
//----------------------------------------------------
//Pre:
//Post: irrelevante
void func2(int n, Semaphore& s1, Semaphore& s2) {

    for(int i=0; i<n; i++) {
        s2.wait();
        cout << "\tpong\n";
        s1.signal();
    }
}
//----------------------------------------------------
//Pre:
//Post: irrelevante
void func1_2(int n, Semaphore* s1, Semaphore* s2) {

    for(int i=0; i<n; i++) {
        s1->wait();
        cout << "ping\n";
        s2->signal();
    }
}
//----------------------------------------------------
//Pre:
//Post: irrelevante
void func2_2(int n, Semaphore* s1, Semaphore* s2) {

    for(int i=0; i<n; i++) {
        s2->wait();
        cout << "\tpong\n";
        s1->signal();
    }
}
//----------------------------------------------------
//Pre:
//Post: irrelevante
void func3(int n, Semaphore &sExt) {

    sExt.wait(5);
    cout << "Me he llevado 5 de golpe\n";
    sExt.signal(3);
    cout << "Dejo 3\n";
    sExt.signal(2);
    cout << "Dejo 2 y me quedo en paz\n";
}
//----------------------------------------------------

int main() {

    //-------- distintos ejemplos de declaración
    Semaphore s1(1),
              s2(2);
    Semaphore* s3;     //puntero a semáforo
    Semaphore* r[2];   //array de dos punteros a semáforo. De momento, no usa constructor
    //----------------------------------------------------
    ADD_EVENT("MAIN_BEGIN");

    thread th_1(&func1, 10, ref(s1), ref(s2)),
           th_2(&func2, 10, ref(s1), ref(s2));

    th_1.join();
    th_2.join();

    //si usamos "new", el programador será responsable de liberar
    //la memoria que se ocupa con el "new", mediante "delete"
    s3 = new Semaphore(1);
    thread th_3, th_4;

    th_3 = thread(&func1, 10, ref(s2), ref(*s3));
    th_4 = thread(&func2, 10, ref(s2), ref(*s3));

    th_3.join();
    th_4.join();
    delete s3;
    cout << "----------- Fin primera fase -----------\n";
    //usamos punteros
    r[0] = new Semaphore(1);
    r[1] = new Semaphore(0);

    thread th_5(&func1_2, 10, r[0], r[1]);
    thread th_6(&func2_2, 10, r[0], r[1]);

    th_5.join();
    th_6.join();

    //¿Por qué en este caso no admite "delete [] r"?
    delete r[0];
    delete r[1];

    cout << "----------- Fin segunda fase -----------\n";
    Semaphore sExt(9); //sem. sin info para eventos

    thread th_7(&func3, 10, ref(sExt));
    th_7.join();

    cout << "----------- Fin tercera fase  -----------\n";

    ADD_EVENT("MAIN_END");
    return 0;
}

