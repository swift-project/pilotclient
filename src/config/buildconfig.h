// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CONFIG_BUILDCONFIG_H
#define SWIFT_CONFIG_BUILDCONFIG_H

#include <QStringList>
#include <QVersionNumber>

namespace swift::config
{
    //! Build configuration, also used to secure VATSIM key
    class CBuildConfig
    {
    public:
        //! with FS9 support?
        static constexpr bool isCompiledWithFs9Support(); // defined in buildconfig_gen.inc.in

        //! with FSX support?
        static constexpr bool isCompiledWithFsxSupport(); // defined in buildconfig_gen.inc.in

        //! with P3D support?
        static constexpr bool isCompiledWithP3DSupport(); // defined in buildconfig_gen.inc.in

        //! with FG support?
        static constexpr bool isCompiledWithFGSupport(); // defined in buildconfig_gen.inc.in

        //! with MSFS support?
        static constexpr bool isCompiledWithMSFSSupport(); // defined in buildconfig_gen.inc.in

       //! with MSFS2024 support?
        static constexpr bool isCompiledWithMSFS2024Support(); // defined in buildconfig_gen.inc.in

        //! with FSUIPC support?
        static constexpr bool isCompiledWithFsuipcSupport(); // defined in buildconfig_gen.inc.in

        //! Compiled with any MS Flight Simulator support (P3D, FSX, FS9)
        static constexpr bool isCompiledWithMsFlightSimulatorSupport();

        //! with XPlane support?
        static constexpr bool isCompiledWithXPlaneSupport(); // defined in buildconfig_gen.inc.in

        //! with any simulator libraries
        static constexpr bool isCompiledWithFlightSimulatorSupport();

        //! Debug build?
        static constexpr bool isDebugBuild();

        //! Release build?
        static constexpr bool isReleaseBuild();

        //! Local build for developers
        static bool isLocalDeveloperDebugBuild();

        //! Running on Windows NT platform?
        static constexpr bool isRunningOnWindowsNtPlatform();

        //! Running on MacOS platform?
        static constexpr bool isRunningOnMacOSPlatform();

        //! Running on Linux platform?
        static constexpr bool isRunningOnLinuxPlatform();

        //! Running on Unix (Linux or Mac OS X) platform
        static constexpr bool isRunningOnUnixPlatform();

        //! Info such as Win32, Win64, MacOs, Linux
        static const QString &getPlatformString();

        //! Info string about compilation (short version)
        static const QString &compiledWithInfoShort();

        //! Info string about compilation (long version)
        static const QString &compiledWithInfoLong();

        //! Executable name for swift GUI, no(!) appendix
        static const QString &swiftGuiExecutableName();

        //! Executable name for swift core, no(!) appendix
        static const QString &swiftCoreExecutableName();

        //! Executable name for swift data, no(!) appendix
        static const QString &swiftDataExecutableName();

#ifdef SWIFT_VATSIM_SUPPORT
        //! VATSIM client id
        static int vatsimClientId(); // defined in buildconfig_gen.cpp.in

        //! VATSIM client key
        static const QString &vatsimPrivateKey(); // defined in buildconfig_gen.cpp.in
#endif

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
} // namespace swift::config

// inline definitions of constexpr methods
#define IN_BUILDCONFIG_H
#include "buildconfig.inc"
#undef IN_BUILDCONFIG_H

#endif // SWIFT_CONFIG_BUILDCONFIG_H
