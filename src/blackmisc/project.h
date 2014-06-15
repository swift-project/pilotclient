#ifndef BLACKMISC_CPROJECT_H
#define BLACKMISC_CPROJECT_H

#include <QString>

namespace BlackMisc
{

    /*!
     * \brief Metadata about the project
     */
    class CProject
    {
    public:
        //! with BlackCore?
        static bool isCompiledWithBlackCore();

        //! with BlackSound?
        static bool isCompiledWithBlackSound();

        //! with BlackInput?
        static bool isCompiledWithBlackInput();

        //! with FSX support?
        static bool isCompiledWithFsxSupport();

        //! with XPlane support?
        static bool isCompiledWithXPlaneSupport();

        //! with any simulator libraries
        static bool isCompiledWithFlightSimulatorSupport();

        //! with GUI?
        static bool isCompiledWithGui();

        //! Compiled with as info string
        static const QString &compiledInfo();

        //! Simulator String info
        static const QString &simulators();

        //! Simulator String info
        static const char *simulatorsChar();

        //! Version info
        static const QString &version();

        //! System's name and version
        static const QString &systemNameAndVersion();

        //! System's name and version
        static const char *systemNameAndVersionChar();

        //! Version major
        static int versionMajor();

        //! Version minor
        static int versionMinor();

    private:
        //! Constructor
        CProject() {}

        //! Split version
        static int getMajorMinor(int index);
    };
}

#endif // guard
