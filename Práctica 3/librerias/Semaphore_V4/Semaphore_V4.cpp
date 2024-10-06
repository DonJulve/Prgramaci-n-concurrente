//*****************************************************************
// File:   Semaphore_V4.cpp
// Author: PSCD-University of Zaragoza
// Date:   octubre 2016
// Coms:   Ver Semaphore_V4.hpp
//         La implementación se entenderá cuando veamos variables
//         condición y monitores
//*****************************************************************

//Información útil para entender la implementación en
//http://en.cppreference.com/w/cpp/thread/condition_variable
//La estudiaremos con más detalle cuando tratemos en la asignatura la parte de "monitores"

#include <Semaphore_V4.hpp>

//----------------------------------------------------------
Semaphore::Semaphore(const int n) {
    assert(n >= 0);           //aborta si se viola la Pre

    count = n;
}
//----------------------------------------------------------
Semaphore::~Semaphore() {
}
//----------------------------------------------------------
void Semaphore::wait() {
    unique_lock<mutex> lck(mtx);

    while(count == 0) {
        cv.wait(lck);
    }
    count--;
}
//----------------------------------------------------------
void Semaphore::signal() {
    unique_lock<mutex> lck(mtx);

    count++;
    cv.notify_all(); //podemos cambiar la semántica con cv.notify_one()
}
//----------------------------------------------------------
void Semaphore::signal(const int n) {
    unique_lock<mutex> lck(mtx);

    count = count+n;
    cv.notify_all();
}
//----------------------------------------------------------
void Semaphore::wait(const int n) {
    unique_lock<mutex> lck(mtx);

    assert(n>0);           //aborta si se viola la Pre

    while(count < n) {
        cv.wait(lck);
    }
    count = count-n;
}
