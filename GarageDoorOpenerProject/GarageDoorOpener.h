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
    static void pollInputs();
    
    //runs the main state machine for controller garage door functions
    void runStateMachine();
    
    //starts running of whole system
    void startSystem();
    
    GarageDoorOpener();
    ~GarageDoorOpener();
    
    //TODO add Motor stuff?
    addMotorAPI(Motor* m);
    
    addIRBeamAPI(IRBeam* i);
    
    addGarageDoorRemote(GarageDoorRemote* gdo);
    
    //ends running of whole system
    void endSystem();
    
private:
    //thread to generate events for main thread statemachine
    pthread_t sigGen = -1;
    
    //flag to sync thread
    int runProgram;
    
    //stateMachine enum
    gdstate_t cur_state;
    
    new_event eventTriggeredLast;
    
    //TODO for queued events?
    
    //composed classes
    IRBeam* irb;
    GarageDoorRemote* remote;
    Motor* moter;
    
};

#endif /* defined(____GarageDoorOpener__) */
