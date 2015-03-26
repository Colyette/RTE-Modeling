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

//For DIO port mapping
#define D_I_O_PORT_LENGTH (1)
#define D_I_O_CONTROL_REGISTER (A_D_BASE_ADDRESS + 0x0b)
#define D_I_O_PORT_A (A_D_BASE_ADDRESS + 0x08)
#define D_I_O_PORT_B (A_D_BASE_ADDRESS + 0x09)

//TODO make A input, B output
#define DIO_DIR		(0x10)	// Sets Port A to input (4)->1, Port B to output

//INPUTS
#define FULLY_OPEN          (0x01)
#define FULLY_CLOSED        (0x02)
#define IR_BEAM_BROKEN      (0x04)
#define OVERCURRENT         (0x08)
#define REMOTE_PUSHBUTTON   (0x10)
//OUTPUTS
#define MOTOR_UP            (0x01)
#define MOTOR_DOWN          (0x02)
#define IR_BEAM_ON          (0x04)
#define SIMULATOR_RESET     (0x10)

enum states { CLOSED, OPEN, STOP, OPENING,CLOSING, ERROR,MAX_STATES} gdstate_t;
enum events { button_press, fully_closed, fully_open, ir_trip, over_current, MAX_EVENTS } new_event;

class GarageDoorOpener {
    
public:
    //TODO I/O stuff
    //handlers
    uintptr_t d_i_o_control_handle ;     // control register for ports A, B, and C
    uintptr_t d_i_o_port_a_handle ;
    uintptr_t d_i_o_port_b_handle ;
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
