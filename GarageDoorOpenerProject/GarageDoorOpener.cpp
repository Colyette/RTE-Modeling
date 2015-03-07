/**
 * \file  GarageDoorOpener.cpp
 * \brief
 *
 * \author Alyssa Colyette
 */

#include "GarageDoorOpener.h"



static void * GarageDoorOpenerHelper(void* instance) {
    GarageDoorOpener c_instance = (GarageDoorOpener) instance;
    c_instance->pollInputs(); //casted to instantiated class and run main thread funct
}

GarageDoorOpener::GarageDoorOpener(){
    sigGen = -1;
    cur_state = CLOSED; //initial state
    eventTriggeredLast = MAX_EVENTS; //no event triggered yet...
    
    runProgram = 1; //set run flag for polling thread
    
}

GarageDoorOpener::~GarageDoorOpener(){
    //TODO if q init, then destroy it
}

/**
 * \brief sets the flag for thread syncro to end the program
 */
void GarageDoorOpener::endSystem(){
    runProgram =0;
}

/**
 * \brief starts the program
 *  program now has two threads
 */
void GarageDoorOpener::startSystem() {
    
    //create event listening thread
    if ( pthread_create(&sigGen, NULL,GarageDoorOpenerHelper,this) ) {
        printf("error creating event thread\n");
        return;
    }
    
    //run state machine
    runStateMachine();
}

static void GarageDoorOpener::pollInputs(){
    int simulatedCount=0;
    while(runProgram){ //some event priority biased
        //poll irbeam
        
        //poll motor outputs
        
        //poll button pressing
        
        
        
        
    }//end program
    
}


void GarageDoorOpener::runStateMachine(){
    
    //TODO figure out how to stay in a state
        switch(cur_state) {
            case CLOSED:
                //TODO
                printf("In CLOSED state\n");
                break;
            case OPEN:
                //TODO
                printf("In OPEN state\n");
                break;
            case STOP:
                //TODO
                printf("In STOP state\n");
                break;
            case OPENING:
                //TODO
                printf("In OPENING state\n");
                break;
            case CLOSING:
                //TODO
                printf("In CLOSING state\n");
                break;
            default:
                //TODO, error shouldn't be in this state
                printf("ERROR state: State machine should be restarted\n");
                break;
        }
    
}

int main() {
    GarageDoorOpener mech();
    //init components
    
    //run statemachine
    
    mech.runSystem();
    
    
    
    
    return 0;
}