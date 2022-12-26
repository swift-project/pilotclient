/* Copyright (C) 2018
 * swift Project Community/Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef IN_BUILDCONFIG_H
#error This file is only to be included by buildconfig.h
#endif

// in-line definitions with qmake substitutions
#include "buildconfig_gen.inc"

//! \cond PRIVATE

namespace BlackConfig
{
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
