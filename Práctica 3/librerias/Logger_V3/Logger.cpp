//*****************************************************************
// File:   Logger.cpp
// Author: J. Ezpeleta-University of Zaragoza
// Date:   march 2017
// Coms:   A (monitor) class for the concurrent access to log files
//*****************************************************************

#include <Logger.hpp>
#include <errno.h>
#include <stdio.h>

//----------------------------------------------------------
Logger::Logger(string logName, int buffSize):
    logName(logName),buffSize(buffSize) {

        assert(buffSize>0);
    echoed = false;
    initializeCommonData();
}

Logger::Logger(string logName, ostream& echo, int buffSize):
    logName(logName),buffSize(buffSize), echo(&echo) {

        assert(buffSize>0);
    echoed = true;
    initializeCommonData();
}

//----------------------------------------------------------
Logger::~Logger() {
    save(); //write pending messages
    delete [] messages; //free buffer memory
}
//----------------------------------------------------------
void Logger::initializeCommonData() {
    getMainID();
    messages = new string[buffSize];
    ticketCounter = 1;
    next = 1;
    nMess = 0;
    createLogFile();
}
//----------------------------------------------------------
void Logger::getMainID() {
    stringstream ss;
    ss << this_thread::get_id();
    mainID = ss.str();
}
//----------------------------------------------------------
void Logger::save() {
    
    logFile.open(logName, ios::out | ios::app);
    if (logFile.fail()) {
        runtime_error("Problems with file "+logName+"\n");
    }
    logFile.exceptions(logFile.exceptions() | ios::failbit | ifstream::badbit);

    for (size_t i=0; i<nMess; i++) {
        logFile << messages[i] << endl;
    }
    nMess = 0;
    logFile.close();
}
//----------------------------------------------------------
void Logger::createLogFile() {

    logFile.open(logName);
    if (logFile.fail()) {
        runtime_error("Problems with file "+logName+"\n");
    }
    logFile.exceptions(logFile.exceptions() | ios::failbit | ifstream::badbit);

    logFile << HEADER << endl;
    logFile.close();
}
//----------------------------------------------------------
//Since attributes in the event are "," separated, negating any of the considered conditions means
//some attribute is not present, and the event maybe useless
//TODO: a more general event structure checking

bool Logger::isCorrect(string mess) {
        return (mess.find(",,") == string::npos && mess[0] != ',' && mess[mess.length()] != ',');
}

void Logger::addMessage(string mess) {

    if (!isCorrect(mess)) {
        //Should I just leave the function in a silent way or output a message?
        // cerr << mess+": malformed event\n";
        return;
    }

    uint64_t myTicket;
    const uint64_t uno(1);
    chrono::nanoseconds ns;
    condition_variable* theCond;
    map<uint64_t,condition_variable*>::iterator it;
    //----------------------------------------------------------
    //Next two sentences are "thread safe"
    //Next sentence provides a unique ticket for the thread
    myTicket = atomic_fetch_add(&ticketCounter, uno);
    ns = chrono::duration_cast<chrono::nanoseconds>(
        chrono::system_clock::now().time_since_epoch()
    );

    //----------------------------------------------------------
    //Ensures mutex access to data
    unique_lock<mutex> lck(mtx);

    //Is it my turn?
    if (myTicket != next) { //No: I have to lock and wait
        theCond = new condition_variable();

        pair<uint64_t,condition_variable*> tickCond(myTicket,theCond);
        waitingMap.insert(tickCond);
        //locking me in my own var condition
        theCond->wait(lck);
        
        //find me, and remove me from the queue
        it = waitingMap.find(myTicket);
        waitingMap.erase(it);
        //My condition var is not needed any more
        delete theCond;
    }
    //It's my turn: myTicket=next
    const char SEP = ',';
    const string ID = "id_" + mainID;

    vector<string> event_list = split(mess, ';');
    for (size_t i=0; i<event_list.size(); i++) {
        stringstream ss;
        ss << ID << SEP << ns.count() << SEP << this_thread::get_id() 
           << SEP << myTicket << SEP << event_list[i];
        messages[nMess] = ss.str();
        nMess++;
        if (echoed) {
            *echo << ss.str() << endl;
        }
    }
    
    next++;
    //If buffer is full, save it
    if (nMess == buffSize) {
        save();
    }
    //Look for the next process
    it = waitingMap.find(next);
    if (it != waitingMap.end()) { //Is the next waiting?
        it->second->notify_one();
    }
}
//----------------------------------------------------------
vector<string> Logger::split(const string& s, char sep) {
   vector<string> elems;
   string el;
   istringstream seqEls(s);
   while (getline(seqEls, el, sep)) {
      elems.push_back(el);
   }
   return elems;
}