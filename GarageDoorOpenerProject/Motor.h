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

#define MOTOR_TIME (10)            //time it takes for the motors to open or close the door

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
    //door is fully open flag
    int open_f;

    //door is fully closed flag
    int closed_f;

    int tickU; //number of ticks till fully up
    

    pthread_t motorThread; //thread for simulating the motor usage
    
    int go; //used for running the motors
    
    
    
};

#endif /* defined(____Motor__) */
