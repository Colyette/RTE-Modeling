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
    if(std::cin.peek() == 'r') {
        //::sleep(1); //idk simulated
        std::cin.get(ch); //grab dat biyatch
//        if (std::cin.peek() == '\n' ) {
//            printf("clearing enter in buff\n");
//            std::cin.get(ch); //grabbing the enter too
//        }
        //std::cin.clear(); //TODO not efficient for queued input
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        //fflush(stdin);
        ret = 1;
    } else{
        //std::cin.clear();
        ret=0;
    }
#endif
    if (ret) {
        printf("GarageDoorRemote:: button pressed\n");
    }
    return ret;
}