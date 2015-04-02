/**
 *  IRBeam.cpp
 * \brief
 *
 * \author Alyssa Colyette
 */


#include "IRBeam.h"
#include <stdio.h>

#ifdef HARDWARE         //for the actual ir hardware interface
#include <stdint.h>  //for uint8_t
#include "common.h"
#include <unistd.h>
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
//    //TODO hardware interface definitions
//    uint8_t output;
//    output = in8(d_i_o_port_a_handle); //read data
//    output |=  IR_BEAM_ON; //set IR Beam bit on
//    out8(d_i_o_port_a_handle, output);
//    return 1;

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
//    //TODO hardware interface definitions
//    uint8_t output;
//    output = in8(d_i_o_port_a_handle); //read data
//    output &=  ~IR_BEAM_ON; //clear IR Beam bit on
//    out8(d_i_o_port_a_handle, output) ;//write to port a
//    ret =1;
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
//    //TODO hardware interface definitions
//    uint8_t input;
//    input= in8(d_i_o_port_b_handle);
//    if (input & IR_BEAM_BROKEN) {   //test IR BEAM broken bit
//        ret =1;
//    }
    
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


