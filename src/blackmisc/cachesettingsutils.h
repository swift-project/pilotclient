/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_CACHESETTINGSUTILS_H
#define BLACKMISC_CACHESETTINGSUTILS_H

#include "blackmisc/applicationinfo.h"
#include "blackmisc/blackmiscexport.h"
#include <QString>

namespace BlackMisc
{
    //! Utils for copying cache and settings content
    class BLACKMISC_EXPORT CCacheSettingsUtils
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
        static QString otherVersionFileName(const BlackMisc::CApplicationInfo &info, const QString &relativeFileName);

        //! Create other version's setting file from "my settings file"
        static QString otherVersionSettingsFileName(const BlackMisc::CApplicationInfo &info, const QString &mySettingFile);

        //! Create other version's cache file from "my cache file"
        static QString otherVersionCacheFileName(const BlackMisc::CApplicationInfo &info, const QString &myCacheFile);

        //! Has the settings file for the given other version?
        static bool hasOtherVersionSettingsFile(const BlackMisc::CApplicationInfo &info, const QString &mySettingFile);

        //! Has the cache file for the given other version?
        static bool hasOtherVersionCacheFile(const BlackMisc::CApplicationInfo &info, const QString &myCacheFile);

        //! Setting JSON object as string
        static QString otherVersionSettingsFileContent(const BlackMisc::CApplicationInfo &info, const QString &mySettingFile);

        //! Cache JSON object as string
        static QString otherVersionCacheFileContent(const BlackMisc::CApplicationInfo &info, const QString &myCacheFile);
    };
} // ns

#endif // guard
