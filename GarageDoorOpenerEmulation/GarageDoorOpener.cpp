/**
 * \file  GarageDoorOpener.cpp
 * \brief
 *
 * \author Alyssa Colyette
 */

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include "common.h"
#include "GarageDoorOpener.h"
//#define QUICK_BUTT_TEST


volatile char input;
struct sigevent event;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
//volatile int inputAvailable = 0;
volatile int inputGrabbed = 0;

//maybe use ISRs for Hardware portion
//const struct sigevent * input_handler(void *arg, int id){
//	//InterruptMask(KBINTR);
//	//inputAvailable = 1;
//	//dude idk how to test this yet
//	return(&event);
//}
//void * input_thread(void *arg){
//	int id;
//	//request i/o privity
//	ThreadCtl(_NTO_TCTL_IO,NULL);
//	event.sigev_notify = SIGEV_INTR;
//	//attach ISR to Keyboard interrupt
//	id = InterruptAttach(KBINTR,&input_handler,NULL, 0, 0 );
//	while(1){
//		InterruptWait(0,NULL);
//		//inputAvailable = 1;
//		//InterruptUnmask(KBINTR,id);
//		inputAvailable = 1;
//	}
//	return NULL;
//}

void * GarageDoorOpenerHelper(void* instance) {
    GarageDoorOpener* c_instance = (GarageDoorOpener*) instance;
    printf("SigGenThread: SigGen going into pollInputs funct\n");
    c_instance->pollInputs(); //casted to instantiated class and run main thread funct
    return 0 ;
}

void * KeyBoardHelper(void * instance){
	GarageDoorOpener* c_instance = (GarageDoorOpener*) instance;
	printf("KBThread: KB is now getting input\n");
	c_instance->getKBInput();
	return 0;
}

GarageDoorOpener::GarageDoorOpener(){
    //sigGen = -1;
    cur_state = CLOSED; //initial state
    //cur_state = CLOSING; //initial state
    eventTriggeredLast = MAX_EVENTS; //no clearevent triggered yet...
    Closing = 0;
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

	//create keyboard listener thread
	if ( pthread_create(&kbLis, NULL,KeyBoardHelper,this) ) {
	        printf("MainThread: error creating Keyboard listener thread thread\n");
	        return;
	}
    //create event listening thread
    if ( pthread_create(&sigGen, NULL,GarageDoorOpenerHelper,this) ) {
        printf("MainThread: error creating sigGen thread\n");
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

void GarageDoorOpener::getKBInput(){
	while(1){
		printf("waiting for input: \n");
//		char in;
//		std::cin.get(in);
		int ch = (std::cin >> std::ws).peek();
		if(ch != EOF){
			char c;
			std::cin >>c;
			//pthread_mutex_lock(&mut);
			inQ.push(c);
			//pthread_mutex_unlock(&mut);
		}
	}
}

void GarageDoorOpener::pollInputs(){
    //int simulatedCount=0;
    printf("SigGenThread: we're polling the inputs now\n");
    while(runProgram){ //some event priority biased

#ifdef QUICK_BUTT_TEST
        //event simulation for now
        printf("SigGenThread:Simulating the inputs\n");
        ::sleep(5);
        eventTriggeredLast = button_press;
        printf("SigGenThread:triggered button_press\n");
        ::sleep(2*MOTOR_TIME);
//        eventTriggeredLast = fully_open;
//        printf("SigGenThread:triggered fully_open\n");
//        ::sleep(5);
        printf("SigGen checking if fully open\n");
        if (motor->receivedFullyOpen() ) {
            printf("SigGen:: it is open\n");
            eventTriggeredLast = fully_open;
        }else{
            printf("SigGen:: its not open\n");
        }
        ::sleep(5);
        eventTriggeredLast = button_press;
        printf("SigGenThread:triggered button_press\n");
        ::sleep(2*MOTOR_TIME);
        printf("SigGen checking if fully closed\n");
//        eventTriggeredLast = fully_closed;
//        printf("SigGenThread:triggered fully_closed\n");
        if (motor->receivedFullyClosed() ) {
            printf("SigGen:: it is closed\n");
            eventTriggeredLast = fully_closed;
        }else{
            printf("SigGen:: its not closed\n");
        }
        
        ::sleep(5);
        printf("SigGenThread:ending program from event generating thread\n");
        endSystem();
    
#else

        delay(1);
        //pthread_mutex_lock(&mut);
        if(inQ.size() > 0){
        	input = inQ.front();
        	inputGrabbed = 0;
        	//printf("input is %c\n",input);
        }
        else{
        	input = '0';
        	inputGrabbed = 1;
        	//printf("no input found\n");
        }
        //pthread_mutex_unlock(&mut);

        //poll irbeam
        if (irb->receivedIRTrip() ){
            //printf("SigGen:: ir trip polled true\n");
            pthread_mutex_lock(&mut);
            eventTriggeredLast = ir_trip;
            pthread_mutex_unlock(&mut);
        }

        //poll motor outputs
        else if (motor->receivedOverCurrent() ) {
             //printf("SigGen:: over current polled true\n");
             pthread_mutex_lock(&mut);
            eventTriggeredLast = over_current;
            pthread_mutex_unlock(&mut);
        }
        else if (motor->receivedFullyOpen() ) {
             //printf("SigGen:: fully open polled true\n");
             pthread_mutex_lock(&mut);
             eventTriggeredLast = fully_open;
             pthread_mutex_unlock(&mut);
        }
        else if (motor->receivedFullyClosed() ) {
             //printf("SigGen:: fully closed polled true\n");
             pthread_mutex_lock(&mut);
             eventTriggeredLast = fully_closed;
             pthread_mutex_unlock(&mut);
        }

        //poll button pressing
        else if (remote->receivedButtonPress() ) {
             //printf("SigGen:: button press polled true\n");
             pthread_mutex_lock(&mut);
             eventTriggeredLast = button_press;
             pthread_mutex_unlock(&mut);
        }
        else {
            //do nothing
        }
        //if((inputGrabbed == 1) && (inQ.size()!=0)){
        if(inQ.size()!=0){
        	inQ.pop();
        	inputGrabbed = 0;
        }
//        else{
//        	inQ.pop();
//        }

#endif
    }//end program
    
}

/**
 * \brief called when an event is triggered, handles the transition associated if
 * transition is for the current state.
 */
void GarageDoorOpener::runStateMachine(){
    printf("GarageDoorOpener::runStateMachine called for event %d\n",eventTriggeredLast);
        switch(cur_state) {
            case CLOSED:
                printf("MainThread:MainThread:In CLOSED state\n");
                switch (eventTriggeredLast) {
                    case button_press:
                        printf("MainThread: rec button press\n");
                        pthread_mutex_lock(&mut);
                        eventTriggeredLast= MAX_EVENTS;
                        pthread_mutex_unlock(&mut);
                        //closed exit
                        cur_state = OPENING;
                        _openingEntry();
                        printf("MainThread: from CLOSED to OPENING\n");
                        break;
                        
                    default:
                        break;
                }
                break;
                
            case OPEN:
                printf("MainThread:In OPEN state\n");
                switch (eventTriggeredLast) {
                    case button_press:
                        printf("MainThread: rec button press\n");
                        pthread_mutex_lock(&mut);
                        eventTriggeredLast = MAX_EVENTS;
                        pthread_mutex_unlock(&mut);
                        //no exit
                        cur_state = CLOSING;
                        _closingEntry();
                        printf("MainThread: from OPEN to CLOSING\n");
                        break;
                    default:
                        break;
                }
                break;
                
            case STOP:
                printf("MainThread:In STOP state\n");
                switch ( eventTriggeredLast) {
                    case button_press:
                        printf("MainThread: rec button press\n");
                        if(Closing){
                        	pthread_mutex_lock(&mut);
                            eventTriggeredLast= MAX_EVENTS;
                            pthread_mutex_unlock(&mut);
                            cur_state = OPENING;
                            _openingEntry();        //entry actions
                            printf("MainThread: from STOP to OPENING\n");
                        }else{
                        	pthread_mutex_lock(&mut);
                            eventTriggeredLast= MAX_EVENTS;
                            pthread_mutex_unlock(&mut);
                            cur_state = CLOSING;
                            _closingEntry();        //entry actions
                            printf("MainThread: from STOP to CLOSING\n");
                        }
                        break;
                    default:
                        break;
                }
                break;
                
            case OPENING:
                printf("MainThread:In OPENING state\n");
                switch (eventTriggeredLast) {
                    case over_current:
                        printf("MainThread: rec overcurrent\n");
                        pthread_mutex_lock(&mut);
                        eventTriggeredLast= MAX_EVENTS;
                        pthread_mutex_unlock(&mut);
                        _openingExit();             //exit actions
                        cur_state = STOP;
                        printf("MainThread: from OPENING to STOP\n");
                        //no entry
                        break;
                        
                    case button_press:
                        printf("MainThread: rec button press\n");
                        pthread_mutex_lock(&mut);
                        eventTriggeredLast= MAX_EVENTS;
                        pthread_mutex_unlock(&mut);
                        _openingExit();             //exit actions
                        cur_state = STOP;
                        printf("MainThread: from OPENING to STOP\n");
                        //no entry
                        break;
                        
                    case fully_open:
                        printf("MainThread: rec fully open sig\n");
                        pthread_mutex_lock(&mut);
                        eventTriggeredLast= MAX_EVENTS;
                        pthread_mutex_unlock(&mut);
                        _openingExit();             //exit actions
                        cur_state = OPEN;
                        _openEntry();               //entry actions
                        printf("MainThread: from OPENING to OPEN\n");
                        break;
                    default:
                        break;
                }
                break;
                
            case CLOSING:
                printf("MainThread:In CLOSING state\n");
                switch (eventTriggeredLast) {
                    case fully_closed:
                        printf("MainThread: rec fully closed sig\n");
                        pthread_mutex_lock(&mut);
                        eventTriggeredLast= MAX_EVENTS;
                        pthread_mutex_unlock(&mut);
                        _closingExit();         //start exit
                        cur_state = CLOSED;
                        _closedEntry();         //start entry
                        printf("MainThread: from CLOSING to CLOSED\n");
                        break;
                        
                    case ir_trip:
                        printf("MainThread: IR b tripin'\n");
                        pthread_mutex_lock(&mut);
                        eventTriggeredLast = MAX_EVENTS;
                        pthread_mutex_unlock(&mut);
                        _closingExit();         //start exit
                        cur_state = OPENING;
                        _openingEntry();        //start entry
                        printf("MainThread: from CLOSING to OPENING\n");
                        break;
                        
                    case over_current:
                        printf("MainThread: rec overcurrent\n");
                        pthread_mutex_lock(&mut);
                        eventTriggeredLast = MAX_EVENTS;
                        pthread_mutex_unlock(&mut);
                        _closingExit();         //start exit
                        cur_state = OPENING;
                        _openingEntry();        //start entry
                        printf("MainThread: from CLOSING to OPENING\n");
                        break;
                        
                    case button_press:
                        printf("MainThread: rec button press\n");
                        pthread_mutex_lock(&mut);
                        eventTriggeredLast = MAX_EVENTS;
                        pthread_mutex_unlock(&mut);
                        _closingExit();         //start exit
                        cur_state = STOP;
                        //no entry
                        printf("MainThread: from CLOSING to STOP\n");
                        break;
                    default:
                        break;
                }
                break;
                
            default:
                //TODO, error shouldn't be in this state
                printf("MainThread: ERROR state. State machine should be restarted\n");
                break;
        }
    //printf("Exiting the main state machine\n");
}

void GarageDoorOpener::addIRBeamAPI(IRBeam* i) {
    irb = i;
}

void GarageDoorOpener::addGarageDoorRemoteAPI(GarageDoorRemote* r){
    remote = r;
}

void GarageDoorOpener::addMotorAPI(Motor* m) {
    motor =m;
}

pthread_t GarageDoorOpener::getSigGen() {
    return sigGen;
}

pthread_t GarageDoorOpener::getKBLis(){
	return kbLis;
}

// ~~~~~~~~~~~~~~~~~~~~~Private mems ~~~~~~~~~~~~~~~~~~~~~
/**
 * \brief Closed actions done on entry
 */
void GarageDoorOpener::_closedEntry(){
    printf("MainThread: _closedEntry\n");
    Closing=0;                  //clearClosing
}

/**
 * \brief Open actions done on entry
 */
void GarageDoorOpener::_openEntry(){
    printf("MainThread: _openEntry\n");
    Closing=1;              //setClosing
}

/**
 * \brief opening actions done on entry
 */
void GarageDoorOpener::_openingEntry(){
    printf("MainThread: _openingEntry\n");
    motor->motorUp();
    Closing = 0;
    
}

/**
 * \brief Opening actions done on exit
 */
void GarageDoorOpener::_openingExit(){
    printf("MainThread: _openExit\n");
    motor->stopMotor();
}

/**
 * \brief Closing actions done on entry
 */
void GarageDoorOpener::_closingEntry(){
    printf("MainThread: _closingEntry\n");
    irb->irBeamOn();
    motor->motorDown();
    Closing =1;
}

/**
 * \brief Closing actions done on exit
 */
void GarageDoorOpener::_closingExit(){
    printf("MainThread: _closingExit\n");
    motor->stopMotor();
    irb->irBeamOff();
}


int main() {
    GarageDoorOpener mech= GarageDoorOpener();
    //init components
    IRBeam irb = IRBeam();
    mech.addIRBeamAPI(&irb);
    
    GarageDoorRemote r= GarageDoorRemote();
    mech.addGarageDoorRemoteAPI(&r);
    
    Motor mot = Motor();
    mech.addMotorAPI(&mot);

    //create the keyboard listening thread
//    if(pthread_create(NULL, NULL, input_thread, NULL)){
//    	printf("MainThread: error creating KBListener thread\n");
//    	return -1;
//    }


    

    //run statemachine
    
    mech.startSystem();
    
    //sleep(10); //sleeping for 10ms now
    
    //mech.endSystem();
    pthread_join(mech.getSigGen(), NULL);
    pthread_join(mech.getKBLis(),NULL);
    
    printf("MainThread:Program exiting now, bysies\n");
    
    pthread_mutex_destroy(&mut);
    

    return 0;
}
