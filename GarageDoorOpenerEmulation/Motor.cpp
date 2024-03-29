/**
 *  Motor.cpp
 *
 * \brief
 * \author Alyssa Colyette
 */


#include "Motor.h"
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
    
Motor::Motor() {
    
    printf("Motor API created\n");
    open_f =0;
    closed_f=0; //though closed we don't need to gen an event initially
    go = 0;
}

Motor::~Motor() {
    printf("Motor API destroyed\n");
}

/**
 * \brief for polling the fully open line
 * \post fully_open =0
 */
int Motor::receivedFullyOpen(){
#ifdef HARDWARE
//    uint8_t input;
//    input= in8(d_i_o_port_b_handle);    //read input reg B
//    if (input & FULLY_OPEN) {           //test fully open signal bit
//        return 1;
//    }
	char ch;

	if(input == 'o'){
		inputGrabbed = 1;
		//printf("\nMotor:got key input: %c\n", input);
		return 1;
	}

#else
    
    if (open_f) {
        open_f =0;
        return 1;
    }
#endif
    return 0;
}

/**
 * \brief for polling the fully closed line
 * \post fully_closed = 0
 */
int Motor::receivedFullyClosed(){
#ifdef HARDWARE
//    uint8_t input;
//    input= in8(d_i_o_port_b_handle);    //read input reg B
//    if (input & FULLY_CLOSED) {         //test fully closed signal bit
//        return 1;
//    }
	char ch;

	if(input == 'c'){
		inputGrabbed = 1;
		//printf("\nMotor:got key input: %c\n", input);
		return 1;
	}
#else
    
    if (closed_f) {
        closed_f =0;
        return 1;
    }
#endif
    return 0;
}

/**
 * \brief for polling the overcurrent line
 */
int Motor::receivedOverCurrent(){
    int ret=0;
#ifdef HARDWARE
//    uint8_t input;
//    input= in8(d_i_o_port_b_handle);    //read input reg
//    if (input & OVERCURRENT) {          //test overcurrent bit
//        ret =1;
//    }
    char ch;

        if(input == 'm'){
        	inputGrabbed = 1;
        	//printf("\nMotor:got key input: %c\n", input);
            ret = 1;
        }else{
            ret=0;
        }
#else

    char ch;

    if(input == 'm'){
    	inputGrabbed = 1;
    	//printf("\nMotor:got key input: %c\n", input);
        ret = 1;
    }else{
        ret=0;
    }

#endif
    if(ret){
    	printf("Motor:: received over current\n");
    }
    return ret;
}

//stop the door motors
int Motor::stopMotor(){
    int ret;
#ifdef HARDWARE
    uint8_t output;
    output = in8(d_i_o_port_b_handle); //read data
    output &=  ~(MOTOR_UP |MOTOR_DOWN); //clear MOTOR UP and DOwn bits
    out8(d_i_o_port_b_handle, output) ;//write to port A
    ret =1;
    
#else
    go =0; // stops the threads at their tick rest
    //pthread_join(motorThread,NULL);
    pthread_cancel(motorThread);
    ret =1;
#endif
    if(ret) {
        printf("Motor:: motors have been stopped\n");
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
    uint8_t output;
    output = in8(d_i_o_port_b_handle); //read data, to not overwrite data
    output |=  MOTOR_UP; //set motor up
    out8(d_i_o_port_b_handle, output); //write to port A
    return 1;

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
 * \brief
 */
void Motor::runOpenSim(){
    ::sleep(MOTOR_TIME);
    open_f =1;
    go=0;
    printf("done MotorUp\n");
}

/**
 * \brief
 */
void Motor::runCloseSim(){
    ::sleep(MOTOR_TIME);
    closed_f =1;
    go=0;
}

/**
 * \brief tells the motor controller to close the door
 * \return whether the signal was sucessfully turned on
 */
int Motor::motorDown(){
#ifdef HARDWARE
    uint8_t output;
    output = in8(d_i_o_port_b_handle); //read data, to not overwrite
    output |=  MOTOR_DOWN; //set motor down
    out8(d_i_o_port_b_handle, output); //write to port A
    return 1;
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

