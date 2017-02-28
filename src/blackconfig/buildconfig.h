/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCONFIG_BUILDCONFIG_H
#define BLACKCONFIG_BUILDCONFIG_H

#include <QList>
#include <QString>

class QDateTime;
class QStringList;

namespace BlackConfig
{
    //! Build configuration
    class CBuildConfig
    {
    public:
        //! with BlackCore?
        static bool isCompiledWithBlackCore(); // defined in buildconfig_gen.cpp.in

        //! with BlackSound?
        static bool isCompiledWithBlackSound(); // defined in buildconfig_gen.cpp.in

        //! with BlackInput?
        static bool isCompiledWithBlackInput(); // defined in buildconfig_gen.cpp.in

        //! with FS9 support?
        static bool isCompiledWithFs9Support(); // defined in buildconfig_gen.cpp.in

        //! with FSX support?
        static bool isCompiledWithFsxSupport(); // defined in buildconfig_gen.cpp.in

        //! with P3D support?
        static bool isCompiledWithP3DSupport(); // defined in buildconfig_gen.cpp.in

        //! Compiled with any MS Flight Simulator support (P3D, FSX, FS9)
        static bool isCompiledWithMsFlightSimulatorSupport();

        //! with XPlane support?
        static bool isCompiledWithXPlaneSupport(); // defined in buildconfig_gen.cpp.in

        //! with any simulator libraries
        static bool isCompiledWithFlightSimulatorSupport();

        //! with GUI?
        static bool isCompiledWithGui(); // defined in buildconfig_gen.cpp.in

        //! Debug build?
        static bool isDebugBuild();

        //! Release build?
        static bool isReleaseBuild();

        //! Beta test?
        static bool isBetaTest(); // defined in buildconfig_gen.cpp.in

        //! Can run in dev. environment?
        static bool canRunInDeveloperEnvironment();

        //! Shipped version?
        static bool isShippedVersion(); // defined in buildconfig_gen.cpp.in

        //! Vatsim enabled version?
        static bool isVatsimVersion(); // defined in buildconfig_gen.cpp.in

        //! Running on Windows NT platform?
        static bool isRunningOnWindowsNtPlatform();

        //! Windows 10
        static bool isRunningOnWindows10();

        //! Running on Mac OS X platform?
        static bool isRunningOnMacOSXPlatform();

        //! Running on Linux platform?
        static bool isRunningOnLinuxPlatform();

        //! Running on Unix (Linux or Mac OS X) platform
        static bool isRunningOnUnixPlatform();

        //! Application directory where current application is located
        static const QString &getApplicationDir();

        //! Where resource files (static DB files, ...) etc are located
        //! \remark share not shared (do no mix)
        static const QString &getSwiftShareDir();

        //! Bootstrap resource directory
        static const QString &getBootstrapResourceFile();

        //! Where static DB files are located
        static const QString &getSwiftStaticDbFilesDir();

        //! Where sound files are located
        static const QString &getSoundFilesDir();

        //! Where qss files are located
        static const QString &getStylesheetsDir();

        //! Where images are located
        static const QString &getImagesDir();

        //! Where HTML files are located
        static const QString &getHtmlDir();

        //! Where Legal files are located
        static const QString &getLegalDir();

        //! The about document file location
        static const QString &getAboutFileLocation();

        //! Where test files are located
        static const QString &getTestFilesDir();

        //! Where HTML files are located
        static const QString &getHtmlTemplateFileName();

        //! Directory where data can be stored
        static const QString &getDocumentationDirectory();

        //! Info string about compilation
        static const QString &compiledWithInfo(bool shortVersion = true);

        //! Executable name for swift GUI, no(!) appendix
        static const QString &swiftGuiExecutableName();

        //! Executable name for swift core, no(!) appendix
        static const QString &swiftCoreExecutableName();

        //! Executable name for swift data, no(!) appendix
        static const QString &swiftDataExecutableName();

        //! End of lifetime
        static const QDateTime &getEol(); // defined in buildconfig_gen.cpp.in

        //! Lifetime ended?
        static bool isLifetimeExpired();

        //! Vatsim client id
        static int vatsimClientId(); // defined in buildconfig_gen.cpp.in

        //! Vatsim client key
        static const QString &vatsimPrivateKey(); // defined in buildconfig_gen.cpp.in
    };

    //! Version
    class CVersion
    {
    public:
        //! Version info
        static const QString &version(); // defined in buildconfig_gen.cpp.in

        //! Version major
        static int versionMajor();

        //! Version minor
        static int versionMinor();

        //! Version patch
        static int versionPatch();

        //! Is the given string representing a newer version?
        static bool isNewerVersion(const QString &versionString);

    private:
        //! Parts of version string 1.0.2
        static QList<int> getVersionParts(const QString &versionString);
    };
}

#endif // guard
