/**
 *  IRBeam.cpp
 * \brief
 *
 * \author Alyssa Colyette
 */


#include "IRBeam.h"

#ifdef HARDWARE         //for the actual ir hardware interface

#else                   //keyboard simulated interface
#include <iostream>
#include <cstdlib>
#include "common.h"
#endif

IRBeam::IRBeam(){
    
    printf("IRBeam API created\n");
    beamCon =0; //default beam off
    
}

IRBeam::~IRBeam() {
    
    printf("IRBeam API destroyed\n");
}

/*
 * \brief Turns on the IR Beam
 *          This should only be called when the door is closing
 * \return success boolean
 */
int IRBeam::irBeamOn() {
    int ret=0;
#ifdef HARDWARE
    //TODO hardware interface definitions
#else
    ret = 1;
#endif
    
    if (ret) {
        printf("irBeam turned on\n");
        beamCon=1;
    }
    return ret;
}

/*
 * \brief Turns off IR Beam
 *          Should not be off when door is closing
 * \return success boolean
 */
int IRBeam::irBeamOff(){
    int ret=0;
#ifdef HARDWARE
    //TODO hardware interface definitions
#else
    ret = 1; 
#endif
    
    if (ret) {
        printf("irBeam turned off\n");
        beamCon=0;
    }
    return ret;
}

/*
 * \brief returns if the beam is tripped or not currently
 * \return high if tripped, 0 if not interrrupted
 * \precon irBreamOn() should be called before polling 
 */
int IRBeam::receivedIRTrip(){
    int ret=0;
#ifdef HARDWARE
    //TODO hardware interface definitions
#else

    char ch;
    if(beamCon) { //check if beam is actually on

    	if(input == 'i'){
        	inputGrabbed = 1;
//    		  printf("\nIRBeam:got key input: %c\n",input);
            ret = 1;
        }
    	else{
            ret=0;
        }
    }
#endif
    if (ret) {
        printf("irBeam interrupted\n");
    }


    return ret;
}


