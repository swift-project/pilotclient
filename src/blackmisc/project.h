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

        //! Beta test?
        static bool isBetaTest();

        //! Running on Windows NT platform?
        static bool isRunningOnWindowsNtPlatform();

        //! Running in dev.environment, so on a programmers machine
        static bool isRunningInDeveloperEnvironment();

        //! Use development setup?
        static bool useDevelopmentSetup();

        //! Beta / dev.environment?
        static bool isRunningInBetaOrDeveloperEnvironment();

        //! Application directory where current application is located
        static const QString &getApplicationDir();

        //! Where resource files (static DB files, ...) etc are located
        static const QString &getSwiftResourceDir();

        //! Private resource dir for developer's own resource files
        static QString getSwiftPrivateResourceDir();

        //! Where static DB files are located
        static const QString &getSwiftStaticDbFilesDir();

        //! Where images are located
        static const QString &getImagesDir();

        //! Dump all env.variables
        static QString getEnvironmentVariables(const QString &separator = QString("\n"));

        //! Info string about compilation
        static const QString &compiledWithInfo(bool shortVersion = true);

        //! Env.information
        static QString environmentInfo(const QString &separator = QString("\n"));

        //! Whole info
        static QString convertToQString(const QString &separator = QString("\n"));

    private:
        //! Constructor
        CProject() {}

        //! Split version
        static int getMajorMinor(int index);

        // --------------- env.vars. -------------
        // centralized in one place here so we have an overview

        //! Environment variable indicating "dev.environment"
        static const QString &envVarDevelopment();

        //! Value
        //! \return true|false
        static QString envVarDevelopmentValue();

        //! Environment variable private resources directory
        static const QString &envVarPrivateSetupDir();

        //! Value
        //! \return directory path
        static QString envVarPrivateSetupDirValue();
    };
} // ns

#endif // guard
