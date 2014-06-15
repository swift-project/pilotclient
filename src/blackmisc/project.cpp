#include "project.h"
#include <QStringList>
#include "blackmisc/blackmiscfreefunctions.h"

#define BLACK_VERSION_STR_X(v) #v
#define BLACK_VERSION_STR(v) BLACK_VERSION_STR_X(v)

namespace BlackMisc
{

    bool CProject::isCompiledWithBlackCore()
    {
#ifdef WITH_BLACKCORE
        return true;
#else
        return false;
#endif
    }

    bool CProject::isCompiledWithBlackSound()
    {
#ifdef WITH_BLACKSOUND
        return true;
#else
        return false;
#endif
    }

    bool CProject::isCompiledWithBlackInput()
    {
#ifdef WITH_BLACKINPUT
        return true;
#else
        return false;
#endif
    }

    bool CProject::isCompiledWithFsxSupport()
    {
#ifdef WITH_FSX
        return true;
#else
        return false;
#endif
    }

    bool CProject::isCompiledWithXPlaneSupport()
    {
#ifdef WITH_XPLANE
        return true;
#else
        return false;
#endif
    }

    bool CProject::isCompiledWithFlightSimulatorSupport()
    {
        return isCompiledWithFsxSupport() || isCompiledWithXPlaneSupport();
    }

    bool BlackMisc::CProject::isCompiledWithGui()
    {
#ifdef WITH_BLACKGUI
        return true;
#else
        return false;
#endif
    }

    const QString &CProject::compiledInfo()
    {
        static QString info;
        if (info.isEmpty())
        {
            static QStringList sl;
            if (isCompiledWithBlackCore()) sl << "BlackCore";
            if (isCompiledWithBlackSound()) sl << "BlackSound";
            if (isCompiledWithBlackInput()) sl << "BlackInput";
            if (isCompiledWithGui()) sl << "BlackGui";
            if (isCompiledWithFsxSupport()) sl << "FSX";
            if (isCompiledWithXPlaneSupport()) sl << "XPlane";
            info = sl.join(", ");
            if (info.isEmpty()) info = "<none>";
        }
        return info;
    }

    const QString &CProject::simulators()
    {
        static QString sims;
        if (sims.isEmpty())
        {
            static QStringList sl;
            if (isCompiledWithFsxSupport()) sl << "FSX";
            if (isCompiledWithXPlaneSupport()) sl << "XPlane";
            sims = sl.join(", ");
            if (sims.isEmpty()) sims = "<none>";
        }
        return sims;
    }

    const char *CProject::simulatorsChar()
    {
        static const QByteArray a(simulators().toUtf8());
        return a.constData();
    }

    const QString &CProject::version()
    {
#ifdef BLACK_VERSION
        const static QString v(BLACK_VERSION_STR(BLACK_VERSION));
#else
        const static QString v("?");
#endif
        return v;
    }

    const QString &CProject::systemNameAndVersion()
    {
        static QString s = QString("swift %1").arg(version());
        return s;
    }

    const char *CProject::systemNameAndVersionChar()
    {
        static const QByteArray a(systemNameAndVersion().toUtf8());
        return a.constData();
    }

    int CProject::versionMajor()
    {
        return getMajorMinor(0);
    }

    int CProject::versionMinor()
    {
        return getMajorMinor(1);
    }

    int CProject::getMajorMinor(int index)
    {
        QString v = version();
        if (v.isEmpty() || !v.contains(".")) return -1;
        bool ok;
        int vi = v.split(".")[index].toInt(&ok);
        return ok ? vi : -1;
    }
}

#undef BLACK_VERSION_STR
#undef BLACK_VERSION_STR_X
