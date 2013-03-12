#ifndef DRIVER_XPLANE_H
#define DRIVER_XPLANE_H

#include <simulator.h>

class CDriverXPlane : public ISimulator
{
public:
    CDriverXPlane();
    virtual ~CDriverXPlane();
    
    virtual int init();
};

#endif // DRIVER_XPLANE_H
