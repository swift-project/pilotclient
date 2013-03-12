#ifndef DRIVER_FS9_H
#define DRIVER_FS9_H

#include <simulator.h>

class CDriverFS9 : public ISimulator
{
public:
    CDriverFS9();
    virtual int init() { return 1;}
	
protected:

    BlackMisc::CLibraryContext *m_libraryContext;
};

#endif // DRIVER_FS9_H
