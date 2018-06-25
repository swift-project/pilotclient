// in P3Dv4 the simconnect.h does not include windows.h
// here we include windows.h first

#ifndef BLACKSIMPLUGIN_FSX_SIMCONNECTWINDOWS_H
#define BLACKSIMPLUGIN_FSX_SIMCONNECTWINDOWS_H

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <SimConnect.h>

#ifndef Q_OS_WIN64
//! adding struct SIMCONNECT_DATA_PBH not existing in SimConnect FSX
struct SIMCONNECT_DATA_PBH
{
    double  Pitch;    //!< pitch
    double  Bank;     //!< bank
    double  Heading;  //!< heading
};
#endif

#endif // guard
