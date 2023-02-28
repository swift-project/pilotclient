/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCONFIG_BUILDCONFIG_H
#define BLACKCONFIG_BUILDCONFIG_H

#include <QList>
#include <QStringList>
#include <QDateTime>
#include <QVersionNumber>

namespace BlackConfig
{
    //! Build configuration, also used to secure VATSIM key
    class CBuildConfig
    {
    public:
        //! with BlackCore?
        static constexpr bool isCompiledWithBlackCore(); // defined in buildconfig_gen.inc.in

        //! with BlackSound?
        static constexpr bool isCompiledWithBlackSound(); // defined in buildconfig_gen.inc.in

        //! with BlackInput?
        static constexpr bool isCompiledWithBlackInput(); // defined in buildconfig_gen.inc.in

        //! with FS9 support?
        static constexpr bool isCompiledWithFs9Support(); // defined in buildconfig_gen.inc.in

        //! with FSX support?
        static constexpr bool isCompiledWithFsxSupport(); // defined in buildconfig_gen.inc.in

        //! with P3D support?
        static constexpr bool isCompiledWithP3DSupport(); // defined in buildconfig_gen.inc.in

        //! with FG support?
        static constexpr bool isCompiledWithFGSupport();  // defined in buildconfig_gen.inc.in

        //! with FSUIPC support?
        static constexpr bool isCompiledWithFsuipcSupport(); // defined in buildconfig_gen.inc.in

        //! Compiled with any MS Flight Simulator support (P3D, FSX, FS9)
        static constexpr bool isCompiledWithMsFlightSimulatorSupport();

        //! with XPlane support?
        static constexpr bool isCompiledWithXPlaneSupport(); // defined in buildconfig_gen.inc.in

        //! with any simulator libraries
        static constexpr bool isCompiledWithFlightSimulatorSupport();

        //! with GUI?
        static constexpr bool isCompiledWithGui(); // defined in buildconfig_gen.inc.in

        //! Debug build?
        static constexpr bool isDebugBuild();

        //! Release build?
        static constexpr bool isReleaseBuild();

        //! Local build for developers
        static bool isLocalDeveloperDebugBuild();

        //! Vatsim enabled version?
        static constexpr bool isVatsimVersion(); // defined in buildconfig_gen.inc.in

        //! Running on Windows NT platform?
        static constexpr bool isRunningOnWindowsNtPlatform();

        //! Windows 10
        static bool isRunningOnWindows10();

        //! Running on MacOS platform?
        static constexpr bool isRunningOnMacOSPlatform();

        //! Running on Linux platform?
        static constexpr bool isRunningOnLinuxPlatform();

        //! Running on Unix (Linux or Mac OS X) platform
        static constexpr bool isRunningOnUnixPlatform();

        //! Info such as Win32, Win64, MacOs, Linux
        static const QString &getPlatformString();

        //! Info string about compilation
        static const QString &compiledWithInfo(bool shortVersion = true);

        //! Executable name for swift GUI, no(!) appendix
        static const QString &swiftGuiExecutableName();

        //! Executable name for swift core, no(!) appendix
        static const QString &swiftCoreExecutableName();

        //! Executable name for swift data, no(!) appendix
        static const QString &swiftDataExecutableName();

        //! Known executable
        static bool isKnownExecutableName(const QString &executable);

        //! Vatsim client id
        static int vatsimClientId(); // defined in buildconfig_gen.cpp.in

        //! Vatsim client key
        static const QString &vatsimPrivateKey(); // defined in buildconfig_gen.cpp.in

        //! Backtrace token for minidump uploads
        static const QString &backtraceToken(); // defined in buildconfig_gen.cpp.in

        //! GitHub repository URL
        static const QString &gitHubRepoUrl();

        //! GitHub Packages REST API URL
        static const QString &gitHubRepoApiUrl();

        //! Returns SHA-1 of git HEAD at build time
        static const QString &gitHeadSha1();

        //! Returns the build date and time as string
        static const QString &buildDateAndTime();

        //! Version as QVersionNumber
        static const QVersionNumber &getVersion();

        //! Version as QVersionNumber
        static const QString &getVersionString();

        //! Version as QVersionNumber
        static const QString &getShortVersionString();

        //! Version as QVersionNumber plus platform info
        static const QString &getVersionStringPlatform();

        //! Build ABI parts as in http://doc.qt.io/qt-5/qsysinfo.html#buildAbi
        static const QStringList &getBuildAbiParts();

        //! Whether this swift application is build as 32 or 64bit application
        //! \returns 32, 64 or -1 (in case no info is possible)
        static int buildWordSize();

    private:
        //! Major version
        static constexpr int versionMajor(); // defined in buildconfig_gen.inc.in

        //! Minor version
        static constexpr int versionMinor(); // defined in buildconfig_gen.inc.in

        //! Patch version
        static int versionRevision(); // defined in buildconfig_gen.cpp.in
    };
} // ns

// inline definitions of constexpr methods
#define IN_BUILDCONFIG_H
#include "buildconfig.inc"
#undef IN_BUILDCONFIG_H

#endif // guard
