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

#define POLL_RATE (250000000)
//handlers
extern uintptr_t d_i_o_control_handle ;     // control register for ports A, B, and C
extern uintptr_t d_i_o_port_a_handle ;
extern uintptr_t d_i_o_port_b_handle ;
extern uintptr_t d_i_o_port_interrupt_handle ;
//#define KBINTR 1	//interrupt vector number for QNX

extern pthread_mutex_t mut;
volatile extern int inputGrabbed;
volatile extern char input; //KEVIN moved this for some reason to garageDoor Opener.cpp


#endif

//main ()
//{
//    // perform initializations, etc.
//    ...
//    // start up a thread that is dedicated to interrupt processing
//    pthread_create (NULL, NULL, int_thread, NULL);
//    ...
//    // perform other processing, as appropriate
//    ...
//}
//
//// this thread is dedicated to handling and managing interrupts
//void *
//int_thread (void *arg)
//{
//    // enable I/O privilege
//    ThreadCtl (_NTO_TCTL_IO, NULL);
//    ...
//    // initialize the hardware, etc.
//    ...
//    // attach the ISR to IRQ 3
//    InterruptAttach (IRQ3, isr_handler, NULL, 0, 0);
//    ...
//    // perhaps boost this thread's priority here
//    ...
//    // now service the hardware when the ISR says to
//    while (1)
//    {
//        InterruptWait (NULL, NULL);
//        // at this point, when InterruptWait unblocks,
//        // the ISR has returned a SIGEV_INTR, indicating
//        // that some form of work needs to be done.
//
//
//        ...
//        // do the work
//        ...
//        // if the isr_handler did an InterruptMask, then
//        // this thread should do an InterruptUnmask to
//        // allow interrupts from the hardware
//    }
//}
//
//// this is the ISR
//const struct sigevent *
//isr_handler (void *arg, int id)
//{
//    // look at the hardware to see if it caused the interrupt
//    // if not, simply return (NULL);
//    ...
//    // in a level-sensitive environment, clear the cause of
//    // the interrupt, or at least issue InterruptMask to
//    // disable the PIC from reinterrupting the kernel
//    ...
//    // return a pointer to an event structure (preinitialized
//    // by main) that contains SIGEV_INTR as its notification type.
//    // This causes the InterruptWait in "int_thread" to unblock.
//    return (&event);
//}
