/**
 * \file GarageDoorRemote.cpp
 * \brief
 * \author Alyssa Colyette
 */

#include "GarageDoorRemote.h"
#include <stdio.h>

#ifdef HARDWARE         //for the actual ir hardware interface
#include <stdint.h>  //for uint8_t
#include "common.h"
#include <unistd.h>

#else                   //keyboard simulated interface
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include "common.h"
#endif

GarageDoorRemote::GarageDoorRemote(){
    printf("GarageDoorRemote API created\n");
}

GarageDoorRemote::~GarageDoorRemote(){
    printf("GarageDoorRemote API destroyed\n");
}

int GarageDoorRemote::receivedButtonPress(){
    int ret;
#ifdef HARDWARE
//    //TODO
//    uint8_t input;
//    input= in8(d_i_o_port_b_handle);    //read input reg B
//    if (input & REMOTE_PUSHBUTTON) {    //test button press bit
//        ret =1;
//    }
    char ch;

	if(input == 'r'){
		inputGrabbed = 1;
    	//printf("\nGarageDoorRemote:got key input: %c\n", input);
    	ret = 1;
	}else{
		//std::cin.clear();
		ret=0;
	}
#else

    char ch;

    if(input == 'r'){
    	inputGrabbed = 1;
    	//printf("\nGarageDoorRemote:got key input: %c\n", input);
        ret = 1;
    }
	else{
        //std::cin.clear();
        ret=0;
    }
#endif
    if (ret) {
        printf("GarageDoorRemote:: button pressed\n");
    }
    return ret;
}
