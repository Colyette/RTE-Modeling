/**
 * \file  GarageDoorOpener.cpp
 * \brief
 *
 * \author Alyssa Colyette
 */

#include <stdio.h>
#include <unistd.h>
#include "GarageDoorOpener.h"



void * GarageDoorOpenerHelper(void* instance) {
    GarageDoorOpener* c_instance = (GarageDoorOpener*) instance;
    printf("SigGenThread: SigGen going into pollInputs funct\n");
    c_instance->pollInputs(); //casted to instantiated class and run main thread funct
    return 0 ;
}

GarageDoorOpener::GarageDoorOpener(){
    //sigGen = -1;
    cur_state = CLOSED; //initial state
    eventTriggeredLast = MAX_EVENTS; //no clearevent triggered yet...
    
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
        printf("MainThread: error creating event thread\n");
        return;
    }
    
    //run state machine
    printf("MainThread:Starting main state machine\n");
    while (runProgram ) {
        if(eventTriggeredLast < MAX_EVENTS) { //an event was triggered by sigGen
            runStateMachine();
        }
    }
    
}

void GarageDoorOpener::pollInputs(){
    int simulatedCount=0;
    printf("SigGenThread: we're polling the inputs now\n");
    while(runProgram){ //some event priority biased
        //poll irbeam
        
        //poll motor outputs
        
        //poll button pressing
        
        
        //event simulation for now
        printf("SigGenThread:Simulating the inputs\n");
        ::sleep(5);
        eventTriggeredLast = button_press;
        printf("SigGenThread:triggered button_press\n");
        ::sleep(5);
        eventTriggeredLast = fully_open;
        printf("SigGenThread:triggered fully_open\n");
        ::sleep(5);
        eventTriggeredLast = button_press;
        printf("SigGenThread:triggered button_press\n");
        ::sleep(5);
        eventTriggeredLast = fully_closed;
        printf("SigGenThread:triggered fully_closed\n");
        
        ::sleep(5);
        printf("SigGenThread:ending program from event generating thread\n");
        endSystem();
        
        
        
    }//end program
    
}

//TODO mute event just triggered
void GarageDoorOpener::runStateMachine(){
    
    
    
        switch(cur_state) {
            case CLOSED:
                //TODO
                printf("MainThread:MainThread:In CLOSED state\n");
                if(eventTriggeredLast == button_press){
                    eventTriggeredLast= MAX_EVENTS;
                    cur_state = OPENING;
                }
                break;
            case OPEN:
                //TODO
                printf("MainThread:In OPEN state\n");
                if(eventTriggeredLast == button_press){
                    eventTriggeredLast= MAX_EVENTS;
                    cur_state = CLOSING;
                }
                break;
            case STOP:
                //TODO
                printf("MainThread:In STOP state\n");
                break;
            case OPENING:
                //TODO
                printf("MainThread:In OPENING state\n");
                if(eventTriggeredLast == fully_open){
                    eventTriggeredLast= MAX_EVENTS;
                    cur_state = OPEN;
                }

                break;
            case CLOSING:
                //TODO
                printf("MainThread:In CLOSING state\n");
                if(eventTriggeredLast == fully_closed){
                    eventTriggeredLast= MAX_EVENTS;
                    cur_state = CLOSED;
                }

                break;
            default:
                //TODO, error shouldn't be in this state
                printf("MainThread: ERROR state. State machine should be restarted\n");
                break;
        }
    //printf("Exiting the main state machine\n");
}

pthread_t GarageDoorOpener::getSigGen() {
    return sigGen;
}

int main() {
    GarageDoorOpener mech= GarageDoorOpener();
    //init components
    
    //run statemachine
    
    mech.startSystem();
    
    //sleep(10); //sleeping for 10ms now
    
    //mech.endSystem();
    pthread_join(mech.getSigGen(), NULL);
    
    printf("MainThread:Program exiting now, bysies\n");
    
    
    
    
    return 0;
}