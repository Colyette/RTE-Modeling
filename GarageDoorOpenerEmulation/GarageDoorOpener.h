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
#include <stdint.h>       /* for uintptr_t */
#include <hw/inout.h>     /* for in*() and out*() functions */
#include <sys/mman.h>     /* for mmap_device_io() */
//API headers
#include "GarageDoorRemote.h"
#include "IRBeam.h"
#include "Motor.h"

#define TIMER_POLL

enum states { CLOSED, OPEN, STOP, OPENING,CLOSING, ERROR,MAX_STATES} gdstate_t;
enum events { button_press, fully_closed, fully_open, ir_trip, over_current, MAX_EVENTS } new_event;

class GarageDoorOpener {
    
public:

    //TODO I/O stuff

    //setup dig io ports after root access gain
    void SetupDIO();
    void TestPorts();

    // need root access to memory map device modules
    int GetRootAccess();
   
    //sets direction of A and B ports on the purple box
    int setPortDirection();
 
    //for the thread polliing inputs to generate events
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
#ifdef TIMER_POLL
    /**
     * 	\brief initializes timer for input polling
     */
    int InitializeTimer(long nsfreq, int pulseid);\
    /**
     * \brief handles changes in incoming inputs on port A
     */
    int input_handler();
    /**
     * \brief triggers polling of input_handler function
     */
    void * input_thread();
#endif

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

    pthread_t dioLis;

    //flag to sync thread
    int runProgram;
    
    //stateMachine enum
    states cur_state;
    
    events eventTriggeredLast;
    std::queue<char> inQ;


    //TODO for queued events?
    
    
    
};

#endif /* defined(____GarageDoorOpener__) */
