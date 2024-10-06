//*****************************************************************
// File:   Logger.hpp
// Author: J. Ezpeleta-University of Zaragoza
// Date:   March 2017
// Coms:   A (monitor) class for the concurrent access to log files
//*****************************************************************

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <sstream>
#include <atomic>
#include <chrono>
#include <map>
#include <vector>
#include <cassert>
#include <string>

using namespace std;

class Logger {
private:
    string mainID;
    //----- for mutex access to monitor functions
    mutex mtx;
    //----- message buffer
    static const int MAX_MESS = 1024; //default buffer bound
    int buffSize; //actual buffer size
    int nMess; //number of messages in the buffer
    string *messages; //message buffer
    //----- to output messages
    string logName; //the name of the log file
    ofstream logFile; //file stream to the log file>
    bool echoed; //Should the message be echoed?
    ostream* echo; //stream for echoed messages
    //----- for handling the order of arrival
    atomic<uint64_t> ticketCounter; //increments one per process and access
    uint64_t next; //next ticket value having access right
    //each waiting process will add the pair <numTicket,condVar> and then lock on the second component
    map<uint64_t,condition_variable*> waitingMap;
    //-----------------------------------------------------
    //Pre: 
    //Post: Creates the log file. If it already exists, its contents are emptied.
    void createLogFile();
    //-----------------------------------------------------
    //Pre: 
    //Post: Called from both constructors. It initialiazes common attributes
    void initializeCommonData();
    //-----------------------------------------------------
    //Pre:
    //Post: Saves the contents of "messages" into the log file. "messages" is cleared
    void save();
    //-----------------------------------------------------
    //Pre:
    //Post: Splits "s" using "sep" as the separator char, and returns
    //      the vector of the components
    vector<std::string> split(const std::string& s, char sep);
    //-----------------------------------------------------
    //log file header, describing event structure
    const string HEADER = "ID,ts,threadID,ticket,event,a1,a2,a3,...";
    //-----------------------------------------------------
    //Fulfills "mainID" with the thread ID of the main function
    void getMainID();
    //Is "mess" a well formed event?
    bool isCorrect(string mess);
    
public:
	//------------------------- constructor
    //Pre:
    //Post: Creates the logger and associates the message ouput to the file "logName"
    //      for non-echoed messages
    Logger(string logName, int buffSize = MAX_MESS);
    //Pre:
    //Post: creates the logger and associates the message ouput to the file "logName".
    //      Messages, besides stored in the file, will be echoed.
    //      Usually, "echo" will be either "clog", "cout" or "cerr", but it is not mandatory
    Logger(string logName, ostream& echo, int buffSize = MAX_MESS);

    //------------------------- destructor
    //Pre:
    //Post: free everything
    ~Logger();

    //-------------------------
    //Pre: 
    //Post: Adds a new message to the log (in a buffered way)
    //      ";" and "\n" chars are separators, indicating different events in the string
    //      Thread access must be in mutex
    void addMessage(string mess);

};

#endif 
