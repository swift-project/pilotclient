/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/applicationinfolist.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"

using namespace BlackConfig;

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc, CApplicationInfo, CApplicationInfoList)

namespace BlackMisc
{
    CApplicationInfoList::CApplicationInfoList() {}

    CApplicationInfoList::CApplicationInfoList(const CSequence<CApplicationInfo> &other) : CSequence<CApplicationInfo>(other)
    {}

    bool CApplicationInfoList::containsApplication(CApplicationInfo::Application application) const
    {
        return this->contains(&CApplicationInfo::getApplication, application);
    }

    int CApplicationInfoList::removeApplication(CApplicationInfo::Application application)
    {
        return this->removeIf(&CApplicationInfo::getApplication, application);
    }

    QStringList CApplicationInfoList::processNames() const
    {
        QStringList names;
        names.reserve(size());
        for (const CApplicationInfo &info : *this)
        {
            names.append(info.getProcessInfo().processName());
        }
        return names;
    }

    int CApplicationInfoList::otherSwiftVersionsFromDataDirectories(bool reinit)
    {
        this->clear();
        const QMap<QString, CApplicationInfo> otherVersions = reinit ?
                                                                  currentApplicationDataDirectoryMapWithoutCurrentVersion() :
                                                                  applicationDataDirectoryMapWithoutCurrentVersion();

        for (const CApplicationInfo &info : otherVersions)
        {
            this->push_back(info);
        }
        return this->size();
    }

    CApplicationInfoList CApplicationInfoList::fromOtherSwiftVersionsFromDataDirectories(bool reinit)
    {
        CApplicationInfoList il;
        il.otherSwiftVersionsFromDataDirectories(reinit);
        return il;
    }

    const QMap<QString, CApplicationInfo> &CApplicationInfoList::applicationDataDirectoryMapWithoutCurrentVersion()
    {
        static const QMap<QString, CApplicationInfo> directories = currentApplicationDataDirectoryMapWithoutCurrentVersion();
        return directories;
    }

    QMap<QString, CApplicationInfo> CApplicationInfoList::currentApplicationDataDirectoryMapWithoutCurrentVersion()
    {
        QMap<QString, CApplicationInfo> directories;
        for (const QFileInfo &info : CSwiftDirectories::currentApplicationDataDirectories())
        {
            // check for myself (the running swift)
            if (caseInsensitiveStringCompare(info.filePath(), CSwiftDirectories::normalizedApplicationDataDirectory())) { continue; }

            // the application info will be written by each swift application started
            // so the application type will always contain that application
            const QString appInfoFile = CFileUtils::appendFilePaths(info.filePath(), CApplicationInfo::fileName());
            const QString appInfoJson = CFileUtils::readFileToString(appInfoFile);
            CApplicationInfo appInfo;
            if (appInfoJson.isEmpty())
            {
                // no JSON means the app no longer exists
                const QString exeDir = CDirectoryUtils::decodeNormalizedDirectory(info.filePath());
                appInfo.setExecutablePath(exeDir);
            }
            else
            {
                appInfo = CApplicationInfo::fromJson(appInfoJson);
            }
            appInfo.setApplicationDataDirectory(info.filePath());
            directories.insert(info.filePath(), appInfo);
        }

        return directories;
    }

    bool CApplicationInfoList::hasOtherSwiftDataDirectories()
    {
        return !applicationDataDirectoryMapWithoutCurrentVersion().isEmpty();
    }
} // ns
