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

        //! Version major
        static int versionMajor();

        //! Version minor
        static int versionMinor();

        //! Is the given string representing a newer version?
        static bool isNewerVersion(const QString &versionString);

        //! Debug build?
        static bool isDebugBuild();

        //! Release build?
        static bool isReleaseBuild();

        //! Beta test?
        static bool isBetaTest();

        //! Can run in dev. environment
        static bool canRunInDeveloperEnvironment();

        //! Shipped version?
        static bool isShippedVersion();

        //! Running on Windows NT platform?
        static bool isRunningOnWindowsNtPlatform();

        //! Application directory where current application is located
        static const QString &getApplicationDir();

        //! Where resource files (static DB files, ...) etc are located
        static const QString &getSwiftResourceDir();

        //! Bootstrap resource directory
        static const QString &getBootstrapResourceFile();

        //! Where static DB files are located
        static const QString &getSwiftStaticDbFilesDir();

        //! Where images are located
        static const QString &getImagesDir();

        //! Directory where data can be stored
        static const QString &getDocumentationDirectory();

        //! Info string about compilation
        static const QString &compiledWithInfo(bool shortVersion = true);

        //! Whole info
        static QString convertToQString(const QString &separator = QString("\n"));

        //! Executable name for swift GUI, no(!) appendix
        static const QString &swiftGuiExecutableName();

        //! Executable name for swift core, no(!) appendix
        static const QString &swiftCoreExecutableName();

        //! Executable name for swift data, no(!) appendix
        static const QString &swiftDataExecutableName();

        //! swift team default servers for DB, bootstrap etc.
        static const QStringList &swiftTeamDefaultServers();

        //! End of lifetime
        static const QDateTime &getEol();

        //! Lifetime ended?
        static bool isLifetimeExpired();

    private:
        //! Constructor
        CProject() {}

        //! Parts of version string 1.0.2
        static QList<int> getVersionParts(const QString &versionString);

        //! Split version
        static int getMajorMinor(int index);
    };
} // ns

#endif // guard
