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

        //! with Fsuipc support?
        static bool isCompiledWithFsuipcSupport(); // defined in buildconfig_gen.cpp.in

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

        //! Local build for developers
        static bool isLocalDeveloperDebugBuild();

        //! Vatsim enabled version?
        static bool isVatsimVersion(); // defined in buildconfig_gen.cpp.in

        //! Running on Windows NT platform?
        static bool isRunningOnWindowsNtPlatform();

        //! Windows 10
        static bool isRunningOnWindows10();

        //! Running on MacOS platform?
        static bool isRunningOnMacOSPlatform();

        //! Running on Linux platform?
        static bool isRunningOnLinuxPlatform();

        //! Running on Unix (Linux or Mac OS X) platform
        static bool isRunningOnUnixPlatform();

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

        //! End of lifetime
        static const QDateTime &getEol(); // defined in buildconfig_gen.cpp.in

        //! Lifetime ended?
        static bool isLifetimeExpired();

        //! Vatsim client id
        static int vatsimClientId(); // defined in buildconfig_gen.cpp.in

        //! Vatsim client key
        static const QString &vatsimPrivateKey(); // defined in buildconfig_gen.cpp.in

        //! Returns SHA-1 of git HEAD at build time
        static const QString &gitHeadSha1();

        //! Timestamp of the last commit (NOT the authored timestamp)
        static const QDateTime &lastCommitTimestamp(); // defined in buildconfig_gen.cpp.in

        //! Build timestamp
        static const QDateTime &buildTimestamp();

        //! Returns the build date and time as string
        static const QString &buildDateAndTime();

        //! Version as QVersionNumber
        static const QVersionNumber &getVersion();

        //! Version as QVersionNumber
        static const QString &getVersionString();

        //! Version as QVersionNumber plus platform info
        static const QString &getVersionStringPlatform();

        //! Turns last commit timestamp into a version number
        static int lastCommitTimestampAsVersionSegment(const QDateTime &lastCommitTimestamp);

        //! Build ABI parts as in http://doc.qt.io/qt-5/qsysinfo.html#buildAbi
        static const QStringList &getBuildAbiParts();

        //! Turn build timestamp into 4th version segment
        static int buildTimestampAsVersionSegment(const QDateTime &buildTimestamp);

        //! Whether this swift application is build as 32 or 64bit application
        //! \returns 32, 64 or -1 (in case no info is possible)
        static int buildWordSize();

    private:
        //! Major version
        static int versionMajor(); // defined in buildconfig_gen.cpp.in

        //! Minor version
        static int versionMinor(); // defined in buildconfig_gen.cpp.in

        //! Patch version
        static int versionMicro(); // defined in buildconfig_gen.cpp.in
    };
} // ns

#endif // guard
