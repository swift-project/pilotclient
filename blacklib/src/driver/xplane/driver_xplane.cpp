#include "driver_xplane.h"

extern "C"
{
    Q_DECL_EXPORT ISimulator* BB_createISimulatorInstance()
    {
        return new CDriverXPlane;
    }
}

CDriverXPlane::CDriverXPlane()
{
}

CDriverXPlane::~CDriverXPlane()
{
}

int CDriverXPlane::init()
{
    return 1;
}

void CDriverXPlane::setLibraryContext(BlackMisc::IContext *context)
{
#ifdef Q_OS_WIN
    bAssert(!BlackMisc::IContext::isContextInitialised());
#endif

    mLibraryContext = new BlackMisc::CLibraryContext(*context);
}
