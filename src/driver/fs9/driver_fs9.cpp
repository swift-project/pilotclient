#include "driver_fs9.h"

extern "C"
{
    Q_DECL_EXPORT ISimulator* BB_createISimulatorInstance ()
    {
        return new CDriverFSX;
    }

    Q_DECL_EXPORT quint32 BB_InterfaceVersionMajor ()
    {
        return ISimulator::InterfaceVersionMajor;
    }

    Q_DECL_EXPORT quint32 BB_InterfaceVersionMinor ()
    {
        return ISimulator::InterfaceVersionMinor;
    }
}


CDriverFS9::CDriverFS9()
{
}

void CDriverFS9::setLibraryContext(BlackMisc::IContext *context)
{
#ifdef Q_OS_WIN
    bAssert(!BlackMisc::IContext::isContextInitialised());
#endif

    m_libraryContext = new BlackMisc::CLibraryContext(*context);
}
