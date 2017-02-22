/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/directoryutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/settingscache.h"

#include <QStandardPaths>

namespace BlackMisc
{
    CSettingsCache::CSettingsCache()
    {}

    CSettingsCache *CSettingsCache::instance()
    {
        static CSettingsCache cache;
        return &cache;
    }

    const QString &CSettingsCache::persistentStore()
    {
        static const QString dir = CFileUtils::appendFilePaths(getCacheRootDirectory(), relativeFilePath());
        return dir;
    }

    BlackMisc::CStatusMessage CSettingsCache::saveToStore(const QString &keyPrefix)
    {
        return saveToFiles(persistentStore(), keyPrefix);
    }

    BlackMisc::CStatusMessage CSettingsCache::saveToStore(const QStringList &keys)
    {
        return saveToFiles(persistentStore(), keys);
    }

    void CSettingsCache::enableLocalSave()
    {
        connect(CSettingsCache::instance(), &CSettingsCache::valuesSaveRequested, CSettingsCache::instance(), &CSettingsCache::saveToStoreByPacket);
    }

    void CSettingsCache::saveToStoreByPacket(const CValueCachePacket &values)
    {
        CStatusMessage status = saveToFiles(persistentStore(), values.toVariantMap());
        CLogMessage::preformatted(status);
    }

    BlackMisc::CStatusMessage CSettingsCache::loadFromStore()
    {
        return loadFromFiles(persistentStore());
    }

    QString CSettingsCache::filenameForKey(const QString &key)
    {
        return CFileUtils::appendFilePaths(persistentStore(), CValueCache::filenameForKey(key));
    }

    const QString CSettingsCache::relativeFilePath()
    {
        static const QString p("/settings/core");
        return p;
    }

    QStringList CSettingsCache::enumerateStore() const
    {
        return enumerateFiles(persistentStore());
    }
}
