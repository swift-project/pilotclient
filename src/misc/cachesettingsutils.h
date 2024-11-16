// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_CACHESETTINGSUTILS_H
#define SWIFT_MISC_CACHESETTINGSUTILS_H

#include <QString>

#include "misc/applicationinfo.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    //! Utils for copying cache and settings content
    class SWIFT_MISC_EXPORT CCacheSettingsUtils
    {
    public:
        //! Setting?
        static bool isSetting(const QString &fileName);

        //! Cache?
        static bool isCache(const QString &fileName);

        //! Path
        static const QString &binSettings();

        //! Path
        static const QString &binData();

        //! Relative setting path
        static QString relativeSettingsPath(const QString &fileName);

        //! Relative cache path
        static QString relativeCachePath(const QString &fileName);

        //! File name for cache/setting
        static QString otherVersionFileName(const swift::misc::CApplicationInfo &info, const QString &relativeFileName);

        //! Create other version's setting file from "my settings file"
        static QString otherVersionSettingsFileName(const swift::misc::CApplicationInfo &info, const QString &mySettingFile);

        //! Create other version's cache file from "my cache file"
        static QString otherVersionCacheFileName(const swift::misc::CApplicationInfo &info, const QString &myCacheFile);

        //! Has the settings file for the given other version?
        static bool hasOtherVersionSettingsFile(const swift::misc::CApplicationInfo &info, const QString &mySettingFile);

        //! Has the cache file for the given other version?
        static bool hasOtherVersionCacheFile(const swift::misc::CApplicationInfo &info, const QString &myCacheFile);

        //! Setting JSON object as string
        static QString otherVersionSettingsFileContent(const swift::misc::CApplicationInfo &info, const QString &mySettingFile);

        //! Cache JSON object as string
        static QString otherVersionCacheFileContent(const swift::misc::CApplicationInfo &info, const QString &myCacheFile);
    };
} // namespace swift::misc

#endif // guard
