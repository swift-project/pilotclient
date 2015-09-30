/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_CPROJECT_H
#define BLACKMISC_CPROJECT_H

#include "blackmiscexport.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include <QString>

namespace BlackMisc
{

    /*!
     * Metadata about the project
     */
    class BLACKMISC_EXPORT CProject
    {
    public:
        //! with BlackCore?
        static bool isCompiledWithBlackCore();

        //! with BlackSound?
        static bool isCompiledWithBlackSound();

        //! with BlackInput?
        static bool isCompiledWithBlackInput();

        //! with FS9 support?
        static bool isCompiledWithFs9Support();

        //! with FSX support?
        static bool isCompiledWithFsxSupport();

        //! with P3D support
        static bool isCompiledWithP3DSupport();

        //! Compiled with any MS Flight Simulator support (P3D, FSX, FS9)
        static bool isCompiledWithMsFlightSimulatorSupport();

        //! with XPlane support?
        static bool isCompiledWithXPlaneSupport();

        //! with any simulator libraries
        static bool isCompiledWithFlightSimulatorSupport();

        //! with GUI?
        static bool isCompiledWithGui();

        //! Info string about compilation
        static const QString &compiledInfo();

        //! Simulator String info
        static const BlackMisc::Simulation::CSimulatorInfo &simulators();

        //! Simulator String info
        static const char *simulatorsChar();

        //! Version info
        static const QString &version();

        //! System's name and version
        static const QString &swiftVersionString();

        //! System's name and version + info if dev.environemnt
        static const QString &swiftVersionStringDevInfo();

        //! System's name and version
        static const char *swiftVersionChar();

        //! Version major
        static int versionMajor();

        //! Version minor
        static int versionMinor();

        //! Debug build?
        static bool isDebugBuild();

        //! Release build?
        static bool isReleaseBuild();

        //! Running on Windows NT platform?
        static bool isRunningOnWindowsNtPlatform();

        //! Running in dev.environment, so on a programmers machine
        static bool isRunningInDeveloperEnvironment();

        //! Application directory where current application is located
        static QString getApplicationDir();

        //! Where resource files (static DB files, ...) etc are located
        static QString getSwiftResourceDir();

        //! Where resource files (static DB files, ...) etc are located
        static QString getSwiftStaticDbFilesDir();

    private:
        //! Constructor
        CProject() {}

        //! Split version
        static int getMajorMinor(int index);
    };
}

#endif // guard
