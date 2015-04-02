/**
 *  IRBeam.h
 * \brief interface for controlling and reading the IR trip beam
 *
 * \author Alyssa Colyette 
 */

#ifndef ____IRBeam__
#define ____IRBeam__

#include <stdio.h>


class IRBeam{
    
public:
    IRBeam();
    ~IRBeam();
    
    //called to turn on IR Beam
    int irBeamOn();
    
    //called to turn off IR Beam
    int irBeamOff();
    
    //returns if the beam is tripped or not
    int receivedIRTrip();
private:
    int beamCon;
    
    
};

#endif /* defined(____IRBeam__) */
