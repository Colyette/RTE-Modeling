/**
 *  Motor.cpp
 *
 * \brief
 * \author Alyssa Colyette
 */


#include "Motor.h"
#include <stdio.h>
#ifdef HARDWARE         //for the actual ir hardware interface

#else                   //keyboard simulated interface
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#endif
    
Motor::Motor() {
    
    printf("Motor API created\n");
    open_f =0;
    closed_f=0; //though closed we don't need to gen an event initially
    go = 0;
    tickU=0; // door is fully closed...
}

Motor::~Motor() {
    printf("Motor API destroyed\n");
}

/**
 * \brief for polling the fully open line
 * \post fully_open =0
 */
int Motor::receivedFullyOpen(){
    if (open_f) {
        open_f =0;
        return 1;
    }
    return 0;
}

/**
 * \brief for polling the fully closed line
 * \post fully_closed = 0
 */
int Motor::receivedFullyClosed(){
    if (closed_f) {
        closed_f =0;
        return 1;
    }
    return 0;
}

/**
 * \brief for polling the overcurrent line
 */
int Motor::receivedOverCurrent(){
    int ret;
#ifdef HARDWARE
    
#else
    char ch;
    if(std::cin.peek() == 'm') {
        //::sleep(1); //idk
        std::cin.get(ch); //grab dat biyatch
//        if (std::cin.peek() == '\n' ) {
//            printf("clearing enter in buff\n");
//            std::cin.get(ch); //grabbing the enter too
//        }
       // std::cin.clear(); //TODO not efficient for queued input
        ret = 1;
    }else{
        ret=0;
    }

#endif
    return ret;
}

//stop the door motors
int Motor::stopMotor(){
    int ret;
#ifdef HARDWARE
    
#else
    go =0; // stops the threads at their tick rest
    //pthread_join(motorThread,NULL);
    pthread_cancel(motorThread);
    ret =1;
#endif
    if(ret) {
        printf("Motor:: motors have been stopped\n");
        printf("Door is opened %d ticks\n",tickU);
    }
    return ret;
}

/**
 * \brief helper funct to go back within the Motor class for a spawned thread to open
 */
void * MotorOpenHelper(void* instance) {
    Motor * c_instance = (Motor*) instance;
    printf("MotorOpenHelper: motorThread going into runOpenSim funct\n");
    c_instance->runOpenSim(); //casted to instantiated class and run thread funct
    return 0 ;
}

/**
 * \brief tells the motor controller to open the door
 * \return whether the signal was turned on
 */
int Motor::motorUp(){
#ifdef HARDWARE
    
#else
    //spawn simulating thread
    printf("Motor:: motorUP\n");
    if(go) {
        return 0;       //thread is already running
    }
    if (pthread_create(&motorThread,NULL,MotorOpenHelper,this) ) {
        printf("MotorError creating motor up thread\n");
        return 0;
    }
    go=1;
    closed_f =0;
    open_f =0;
    return 1; //success
#endif
}

/**
 * \brief helper funct to go back within the Motor class for a spawned thread to close
 */
void * MotorCloseHelper(void* instance) {
    Motor * c_instance = (Motor*) instance;
    printf("MotorOpenHelper: motorThread going into runCloseSim funct\n");
    c_instance->runCloseSim(); //casted to instantiated class and run thread funct
    return 0 ;
}

/**
 * \brief Ticks for a second, and increments tick to account for it
 *  for the duration of moving the door fully open
 */
void Motor::runOpenSim(){
     //::sleep(MOTOR_TIME);//THIS works a lone but no partial open status allowed in program
    
    //time up already, go the difference
    printf("Ticks left til open:%d\n", MOTOR_TIME-tickU);
    for (int i = MOTOR_TIME -tickU; i < MOTOR_TIME; i++) {
//printf("%d\n",tickU);
        ::sleep(1);       //TODO have a 1sec timer pulse
        tickU++; //went up for a second more
        //TODO make sure the time doesn't get higher than expected
    }
   
    open_f =1;
    go=0;
}

/**
 * \brief Ticks for a seconde and decrements the tick to account for it
 *  for the duration of moving the door fully closed
 */
void Motor::runCloseSim(){
     //::sleep(MOTOR_TIME);//THIS works a lone but no partial open status allowed in program
    
    //time went up is time to go down
    printf("Ticks left til closed:%d\n", tickU);
    for ( int i = tickU; i>0; i--) {
//printf("%d\n",tickU);
        ::sleep(1);       //TODO have a 1sec timer pulse
        tickU--;    // went down for a second more
        //TODO not go lower than 0...
    }
    
    closed_f =1;
    go=0;
    
}

/**
 * \brief tells the motor controller to close the door
 * \return whether the signal was sucessfully turned on
 */
int Motor::motorDown(){
#ifdef HARDWARE
    
#else
    //spawn simulating thread
    printf("Motor:: motorDown\n");
    if(go) {
        return 0;       //thread is already going
    }
    if (pthread_create(&motorThread,NULL,MotorCloseHelper,this) ) {
        printf("Error creating motor up thread\n");
        return 0;
    }
    closed_f =0;
    open_f =0;
    go=1;
    return 1; //success
#endif
}

