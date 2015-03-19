/**
 * \file GarageDoorOpener.h
 * \brief Runs the main controller for the Garage Door Opeining system
 *
 * \author Alyssa Colyette
 */

#ifndef ____GarageDoorOpener__
#define ____GarageDoorOpener__

#include <stdio.h>
#include <pthread.h>    //for threading
#include <queue>
#include <deque>
//API headers
#include "GarageDoorRemote.h"
#include "IRBeam.h"
#include "Motor.h"


enum states { CLOSED, OPEN, STOP, OPENING,CLOSING, ERROR,MAX_STATES} gdstate_t;
enum events { button_press, fully_closed, fully_open, ir_trip, over_current, MAX_EVENTS } new_event;

class GarageDoorOpener {
    
public:
    
    
    //for the thread polliing inputs to generate events
    //TODO might need a helper function
    //TODO might manage queue to update eventTriggeredLast
    void pollInputs();
    

    //runs the main state machine for controller garage door functions
    void runStateMachine();
    
    //starts running of whole system
    void startSystem();
    
    GarageDoorOpener();
    ~GarageDoorOpener();
    
    //TODO add Motor stuff?
    void addMotorAPI(Motor* m);
    
    void addIRBeamAPI(IRBeam* i);
    
    void addGarageDoorRemoteAPI(GarageDoorRemote* gdo);
    
    //ends running of whole system
    void endSystem();
    
    //returns the event creator's thread ID
    pthread_t getSigGen();
    pthread_t getKBLis();
    
    void getKBInput();
    //composed classes
    Motor* motor;
    IRBeam* irb;
    GarageDoorRemote* remote;
private:
    void _closedEntry();
    //void _closedExit();
    
    void _openEntry();
    //void _openExit();
    
    //void _stopEntry();
    //void _stopExit();
    
    void _openingEntry();
    void _openingExit();
    
    void _closingEntry();
    void _closingExit();
    
    int Closing;
    
    
    //thread to generate events for main thread statemachine
    pthread_t sigGen;
    
    //thread for keyboard listener
    pthread_t kbLis;

    //flag to sync thread
    int runProgram;
    
    //stateMachine enum
    states cur_state;
    
    events eventTriggeredLast;
    std::queue<char> inQ;


    //TODO for queued events?
    
    
    
};

#endif /* defined(____GarageDoorOpener__) */
