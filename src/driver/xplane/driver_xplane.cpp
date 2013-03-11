#include "driver_xplane.h"

extern "C"
{
    Q_DECL_EXPORT ISimulator* BB_createISimulatorInstance ()
    {
        return new CDriverXPlane;
    }
}


CDriverXPlane::CDriverXPlane()
{
}

void CDriverFSX::setLibraryContext(BlackMisc::IContext *context)
{
#ifdef Q_OS_WIN
    bAssert(!BlackMisc::IContext::isContextInitialised());
#endif

    m_libraryContext = new BlackMisc::CLibraryContext(*context);
}
