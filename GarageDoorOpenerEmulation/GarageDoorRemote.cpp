/**
 * \file GarageDoorRemote.cpp
 * \brief
 * \author Alyssa Colyette
 */

#include "GarageDoorRemote.h"
#ifdef HARDWARE         //for the actual ir hardware interface

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
    //TODO
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
