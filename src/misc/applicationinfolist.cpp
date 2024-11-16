// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/applicationinfolist.h"

#include "config/buildconfig.h"
#include "misc/directoryutils.h"
#include "misc/stringutils.h"
#include "misc/swiftdirectories.h"

using namespace swift::config;

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc, CApplicationInfo, CApplicationInfoList)

namespace swift::misc
{
    CApplicationInfoList::CApplicationInfoList() {}

    CApplicationInfoList::CApplicationInfoList(const CSequence<CApplicationInfo> &other)
        : CSequence<CApplicationInfo>(other)
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
        for (const CApplicationInfo &info : *this) { names.append(info.getProcessInfo().processName()); }
        return names;
    }

    int CApplicationInfoList::otherSwiftVersionsFromDataDirectories(bool reinit)
    {
        this->clear();
        const QMap<QString, CApplicationInfo> otherVersions =
            reinit ? currentApplicationDataDirectoryMapWithoutCurrentVersion() :
                     applicationDataDirectoryMapWithoutCurrentVersion();

        for (const CApplicationInfo &info : otherVersions) { this->push_back(info); }
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
        static const QMap<QString, CApplicationInfo> directories =
            currentApplicationDataDirectoryMapWithoutCurrentVersion();
        return directories;
    }

    QMap<QString, CApplicationInfo> CApplicationInfoList::currentApplicationDataDirectoryMapWithoutCurrentVersion()
    {
        QMap<QString, CApplicationInfo> directories;
        for (const QFileInfo &info : CSwiftDirectories::currentApplicationDataDirectories())
        {
            // check for myself (the running swift)
            if (caseInsensitiveStringCompare(info.filePath(), CSwiftDirectories::normalizedApplicationDataDirectory()))
            {
                continue;
            }

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
            else { appInfo = CApplicationInfo::fromJson(appInfoJson); }
            appInfo.setApplicationDataDirectory(info.filePath());
            directories.insert(info.filePath(), appInfo);
        }

        return directories;
    }

    bool CApplicationInfoList::hasOtherSwiftDataDirectories()
    {
        return !applicationDataDirectoryMapWithoutCurrentVersion().isEmpty();
    }
} // namespace swift::misc
