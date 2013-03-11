#ifndef DRIVER_XPLANE_H
#define DRIVER_XPLANE_H

#include <simulator.h>

class CDriverXPlane : public ISimulator
{
public:
    CDriverXPlane();
    virtual int init() {return 1;}
	
protected:

    BlackMisc::CLibraryContext *m_libraryContext;
};

#endif // DRIVER_XPLANE_H
