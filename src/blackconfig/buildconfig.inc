// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef IN_BUILDCONFIG_H
#error This file is only to be included by buildconfig.h
#endif

// in-line definitions with qmake substitutions
#include "buildconfig_gen.inc"

//! \cond PRIVATE

namespace BlackConfig
{
    constexpr bool CBuildConfig::isCompiledWithMsFlightSimulatorSupport()
    {
        return CBuildConfig::isCompiledWithFs9Support() || CBuildConfig::isCompiledWithFsxSupport() || CBuildConfig::isCompiledWithP3DSupport();
    }

    constexpr bool CBuildConfig::isCompiledWithFlightSimulatorSupport()
    {
        return CBuildConfig::isCompiledWithFsxSupport() || CBuildConfig::isCompiledWithXPlaneSupport();
    }

    constexpr bool CBuildConfig::isRunningOnWindowsNtPlatform()
    {
#ifdef Q_OS_WIN
        return true;
#else
        return false;
#endif
    }

    constexpr bool CBuildConfig::isRunningOnMacOSPlatform()
    {
#ifdef Q_OS_MACOS
        return true;
#else
        return false;
#endif
    }

    constexpr bool CBuildConfig::isRunningOnLinuxPlatform()
    {
#ifdef Q_OS_LINUX
        return true;
#else
        return false;
#endif
    }

    constexpr bool CBuildConfig::isRunningOnUnixPlatform()
    {
        return CBuildConfig::isRunningOnMacOSPlatform() || CBuildConfig::isRunningOnLinuxPlatform();
    }

    constexpr bool CBuildConfig::isDebugBuild()
    {
#ifdef QT_DEBUG
        return true;
#else
        return false;
#endif
    }

    constexpr bool CBuildConfig::isReleaseBuild()
    {
#ifdef QT_NO_DEBUG
        return true;
#else
        return false;
#endif
    }
} // ns

//! \endcond
