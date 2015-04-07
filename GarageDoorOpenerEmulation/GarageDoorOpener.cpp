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
#include <errno.h>
#include "common.h"
#include <sys/syspage.h> //qtime
#include <assert.h> //sddrt
#include <hw/inout.h>     /* for in*() and out*() functions */
#include <sys/neutrino.h> /* for ThreadCtl() */
#include <sys/mman.h>     /* for mmap_device_io() */
#include <sys/netmgr.h>
#include <stdlib.h> //exit()
#include "GarageDoorOpener.h"
//#define QUICK_BUTT_TEST
#define TIMER_POLL

//volatile char input;
struct sigevent event;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
//volatile int inputAvailable = 0;
volatile int inputGrabbed = 0;
uint8_t old_inputs =0;
volatile char input = 0;

//handler instantiation
uintptr_t d_i_o_control_handle = -1 ;     // control register for ports A, B, and C
uintptr_t d_i_o_port_a_handle = -1 ;
uintptr_t d_i_o_port_b_handle =-1 ;
uintptr_t d_i_o_port_interrupt_handle= -1 ;

//maybe use ISRs for Hardware portion
#ifndef TIMER_POLL
const struct sigevent * input_handler(void *arg, int id){
#else
int GarageDoorOpener::input_handler(){
#endif
	char tempChar; //used to fill up character Q for trig each event
	int swtchCnt=0; //counts the number of rising edges
#ifndef TIMER_POLL
	InterruptMask(DIO_IRQ,id);
#endif
	//check all of the inputs
	uint8_t new_inputs = in8(d_i_o_port_a_handle);
	//printf("o:%x,n:%x\n",old_inputs,new_inputs);
	//check FULLY_OPEN
	if( (old_inputs & FULLY_OPEN)== 0){
		if ( (new_inputs & FULLY_OPEN)==FULLY_OPEN ) {
			//rising edge on Fully open pin
			//TODO
			tempChar = 'o'; //character for fully open GD
			inQ.push(tempChar);

			swtchCnt++;
		}
	}

	//check FULLY_CLOSED
	if( (old_inputs & FULLY_CLOSED)== 0){
		if ( (new_inputs & FULLY_CLOSED)==FULLY_CLOSED ) {
			//rising edge on FULLY_CLOSED pin
			//TODO
			tempChar = 'c'; //character for fully closed GD
			inQ.push(tempChar);
			swtchCnt++;
		}
	}

	//IR_BEAM_BROKEN
	if( (old_inputs & IR_BEAM_BROKEN)== 0){
		if ( (new_inputs & IR_BEAM_BROKEN)==IR_BEAM_BROKEN ) {
			//rising edge on FULLY_CLOSED pin
			//TODO
			tempChar = 'i';
			inQ.push(tempChar);
			swtchCnt++;
		}
	}

	//OVERCURRENT
	if( (old_inputs & OVERCURRENT)== 0){
		if ( (new_inputs & OVERCURRENT)==OVERCURRENT ) {
			//rising edge on OVERCURRENT pin
			//TODO
			tempChar = 'm';
			inQ.push(tempChar);
			swtchCnt++;
		}
	}

	//REMOTE_PUSHBUTTON
	if( (old_inputs & REMOTE_PUSHBUTTON)== 0){
		if ( (new_inputs & REMOTE_PUSHBUTTON)==REMOTE_PUSHBUTTON ) {
			//rising edge on OVERCURRENT pin
			//TODO
			tempChar = 'r';
			inQ.push(tempChar);
			swtchCnt++;
		}
	}

	//change old inputs not
	old_inputs = new_inputs; //TODO may need to account for old input rise handled
#ifndef TIMER_POLL
	InterruptUnmask(DIO_IRQ,id);
	return(&event);
#else
	return swtchCnt;
#endif


}

int GarageDoorOpener::InitializeTimer(long nsfreq, int pulseid){
  int chid;
  timer_t timer_id;
  struct sigevent event;
  struct itimerspec timer;
  printf("Initializing Pulses at %ldns with pulse id as %d\n", nsfreq, pulseid);

  /* Create a channel to receive timer events on. */
  chid = ChannelCreate( 0 ); //b: opening up a channel to send messages through
  assert ( chid != -1 );      // if returns a -1 for failure we stop with error
  /* Set up the timer and timer event. */
  event.sigev_notify = SIGEV_PULSE;   // most basic message we can send -- just a pulse number
  event.sigev_coid = ConnectAttach ( ND_LOCAL_NODE, 0, chid, 0, 0 );  // Get ID that allows me to communicate on the channel
  assert ( event.sigev_coid != -1 );    // stop with error if cannot attach to channel
  event.sigev_priority = getprio(0);
  event.sigev_value.sival_int = pulseid;

  // Now create the timer and get back the timer_id value for the timer we created.
  if ( timer_create( CLOCK_REALTIME, &event, &timer_id ) == -1 )  // CLOCK_REALTIME available in all POSIX systems
  {
    perror ( "cannot create timer\n" );
    exit( EXIT_FAILURE );
  }

  /* Change the timer request to alter the behavior. */
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_nsec = nsfreq;    // interrupt nsfreq -> period
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_nsec = nsfreq; // keep interrupting nsfreq

  /* Start the timer. */
  if ( timer_settime( timer_id, 0, &timer, NULL ) == -1 )
  {
    perror("cannot start timer.\n");
    exit( EXIT_FAILURE );
  }

  return chid;
}

#ifndef TIMER_POLL
void * input_thread(void *arg){
#else
void * GarageDoorOpener::input_thread(){
#endif
#ifndef TIMER_POLL
	int id,err;
	printf("input_thread created\n");
	//request i/o privity
	if ( ThreadCtl(_NTO_TCTL_IO,NULL)==-1) {
			 fprintf(stderr, "failure\n");
	}
	event.sigev_notify = SIGEV_INTR;
	printf(".");
	//attach ISR to Keyboard interrupt
	id = InterruptAttach(DIO_IRQ,&input_handler,NULL, 0, 0 );
	printf(".");
	while(1){

		InterruptWait(0,NULL);
		//InterruptUnmask(DIO_IRQ,id);
		printf("input:%c\n",input);
	}
	return NULL;

#else
	int cnt;
	printf("input_thread created\n");
	struct _pulse pulse; //for timer msg
	int chid,pid; //for target of timer msg passing
	chid= InitializeTimer(POLL_RATE,1); //returns the ch id for send messages
	while (1) {
		pid = MsgReceivePulse(chid,&pulse,  sizeof( pulse ),NULL);
		//printf("#\n");
		cnt = input_handler();
		if (cnt) { //a change in DIO was recognized
			//printf("%d\n",cnt);
		}
	}
	return NULL;
#endif
}

void * DIOListenerHelper(void* instance) {
    GarageDoorOpener* c_instance = (GarageDoorOpener*) instance;
    printf("DIOListnerHelper: dioLis going into input_thread() funct\n");
    c_instance->input_thread(); //casted to instantiated class and run main thread funct
    return 0 ;
}

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

/**
 * \brief sets up digital IO on port A and port B,  requires root access for mapping
 */
void GarageDoorOpener::SetupDIO() {
    d_i_o_control_handle = mmap_device_io( D_I_O_PORT_LENGTH, D_I_O_CONTROL_REGISTER ) ;
    d_i_o_port_a_handle = mmap_device_io( D_I_O_PORT_LENGTH, D_I_O_PORT_A ) ;
    d_i_o_port_b_handle = mmap_device_io( D_I_O_PORT_LENGTH, D_I_O_PORT_B ) ;
    d_i_o_port_interrupt_handle =mmap_device_io( D_I_O_PORT_LENGTH, D_I_O_INTERRUPT_DMA_COUNTER_CONTROL );
}

/**
 * \brief gets root access for the current thread
 */
int GarageDoorOpener::GetRootAccess(){
    int status = 0 ;
    int privity_err ;

    /* Give this thread root permissions to access the hardware */
    privity_err = ThreadCtl( _NTO_TCTL_IO, NULL );
    if ( privity_err == -1 )
    {
        fprintf( stderr, "can't get root permissions\n" );
        status = -1;
    }

    return status ;
}

/**
 * \brief
 */
int GarageDoorOpener::setPortDirection(){
	//if ( !GetRootAccess()){
        SetupDIO();
        out8( d_i_o_control_handle, DIO_DIR) ;     // make port A input,B output
        out8( d_i_o_port_interrupt_handle, DINTE);
        printf("Set A as input and B as output\n");
        return 1;
//    }else {
//        printf("Couldn't get root access to set DIO ports\n");
//        return (-1);
//    }
    
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
#ifndef HARDWARE
	//create keyboard listener thread
	if ( pthread_create(&kbLis, NULL,KeyBoardHelper,this) ) {
	        printf("MainThread: error creating Keyboard listener thread thread\n");
	        return;
	}
#endif
	//create event listening thread
    if ( pthread_create(&sigGen, NULL,GarageDoorOpenerHelper,this) ) {
        printf("MainThread: error creating sigGen thread\n");
        return;
    }
    
    // X create IRQ thread
    // start up a thread that is dedicated to interrupt processing
     if (pthread_create (&dioLis, NULL, DIOListenerHelper, this)) { //TODO thread id
    	 printf("MainThread: error creating dio processing thread\n");
    	 return;
     }

#ifdef HARDWARE
     //reset FPGA
     uint8_t before = in8(d_i_o_port_b_handle);
     before =0; //set low
     out8(d_i_o_port_b_handle,before);
     delay(1);
     before = in8(d_i_o_port_b_handle);
     printf("FPGA inputs set 0x%x\n", before);
     before |= SIMULATOR_RESET; //set high reset
     out8(d_i_o_port_b_handle,before);

#endif

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
                    	pthread_mutex_lock(&mut);
                    	eventTriggeredLast = MAX_EVENTS;
                    	pthread_mutex_unlock(&mut);
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

    if (!mech.GetRootAccess()){
    	mech.setPortDirection();

    	mech.startSystem();
    }else {
    	return 0;
    }
    

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
