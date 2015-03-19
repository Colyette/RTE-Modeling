/**
 * \file Motor.h
 *
 * \brief  Interface for the Motor controller of the GarageDoorOpener
 * \author Alyssa Colyette
 *
 */


#ifndef ____Motor__
#define ____Motor__

#include <stdio.h>
#include <pthread.h>

#define MOTOR_TIME 5            //time it takes for the motors to open or close the door

class Motor{
public:
    Motor();
    ~Motor();
    
    //for polling the fully open line
    int receivedFullyOpen();
    
    //for polling the fully closed line
    int receivedFullyClosed();
    
    //for polling the overcurrent line
    int receivedOverCurrent();
    
    //stop the door motors
    int stopMotor();
    
    //tells the motor controller to open the door
    int motorUp();
    
    //tells the motor controller to close the door
    int motorDown();
    
    //runs for 10s unless go flag is clear
    void runOpenSim();
    
    //runs for 10ms unless go flag is clear
    void runCloseSim();
    
private:
    int open_f;
    int closed_f;
    
    pthread_t motorThread; //thread for simulating the motor usage
    
    int go; //used for running the motors
    
    
    
};

#endif /* defined(____Motor__) */
