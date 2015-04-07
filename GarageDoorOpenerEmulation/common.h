#ifndef __common_h__
#define __common_h__

#include <pthread.h>
#include <sys/siginfo.h>
#include <sys/neutrino.h>

#include <stdint.h>       /* for uintptr_t */
#include <hw/inout.h>     /* for in*() and out*() functions */
#include <sys/mman.h>     /* for mmap_device_io() */

#define HARDWARE //TODO

//For DIO port mapping
#define A_D_BASE_ADDRESS (0x280)
#define D_I_O_PORT_LENGTH (1)
#define D_I_O_CONTROL_REGISTER (A_D_BASE_ADDRESS + 0x0b)
#define D_I_O_INTERRUPT_DMA_COUNTER_CONTROL (A_D_BASE_ADDRESS +0x04)
#define D_I_O_PORT_A (A_D_BASE_ADDRESS + 0x08)
#define D_I_O_PORT_B (A_D_BASE_ADDRESS + 0x09)

//TODO make A input, B output
#define DIO_DIR		(0x10)	// Sets Port A to input (4)->1, Port B to output

//IRQ vector number for DIO
#define DIO_IRQ (0x05)
//enable DIO interrupt
#define DINTE (0x02)

//INPUTS
#define FULLY_OPEN          (0x01)
#define FULLY_CLOSED        (0x02)
#define IR_BEAM_BROKEN      (0x04)
#define OVERCURRENT         (0x08)
#define REMOTE_PUSHBUTTON   (0x10)
//OUTPUTS
#define MOTOR_UP            (0x01)
#define MOTOR_DOWN          (0x02)
#define IR_BEAM_ON          (0x04)
#define SIMULATOR_RESET     (0x08)
//poll rate for the inputs of port A
#define POLL_RATE (250000000)
//handlers
extern uintptr_t d_i_o_control_handle ;     // control register for ports A, B, and C
extern uintptr_t d_i_o_port_a_handle ;
extern uintptr_t d_i_o_port_b_handle ;
extern uintptr_t d_i_o_port_interrupt_handle ;
//#define KBINTR 1	//interrupt vector number for QNX

extern pthread_mutex_t mut;
volatile extern int inputGrabbed; //used to signal input recognized
volatile extern char input;			//next character on q


#endif //end __common_h__


