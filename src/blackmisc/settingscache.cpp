/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/settingscache.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/logmessage.h"

#include <QStandardPaths>

namespace BlackMisc
{
    CSettingsCache::CSettingsCache() : CValueCache(0)
    {}

    CSettingsCache *CSettingsCache::instance()
    {
        static std::unique_ptr<CSettingsCache> cache(new CSettingsCache);
        static auto dummy = (connect(qApp, &QObject::destroyed, cache.get(), [] { cache.reset(); }), nullptr);
        Q_UNUSED(dummy) // declared as static to get thread-safe initialization
        return cache.get();
    }

    const QString &CSettingsCache::persistentStore()
    {
        static const QString dir = CFileUtils::appendFilePaths(getCacheRootDirectory(), relativeFilePath());
        return dir;
    }

    const QString &CSettingsCache::lockFileName()
    {
        static const QString file = CFileUtils::appendFilePaths(persistentStore(), ".lock");
        return file;
    }

    CStatusMessage CSettingsCache::lockFile(QLockFile &lock)
    {
        Q_ASSERT(!lock.isLocked());
        if (!QDir::root().mkpath(persistentStore()))
        {
            return CStatusMessage(this).error(u"Failed to create %1") << persistentStore();
        }
        if (!lock.lock())
        {
            return CStatusMessage(this).error(u"Failed to lock %1: %2") << lockFileName() << CFileUtils::lockFileError(lock);
        }
        return {};
    }

    CStatusMessage CSettingsCache::saveToStore(const QString &keyPrefix)
    {
        QLockFile lock(lockFileName());
        const CStatusMessage lockStatus = lockFile(lock);
        if (lockStatus.isFailure()) { return lockStatus; }

        return saveToFiles(persistentStore(), keyPrefix);
    }

    CStatusMessage CSettingsCache::saveToStore(const QStringList &keys)
    {
        QLockFile lock(lockFileName());
        const CStatusMessage lockStatus = lockFile(lock);
        if (lockStatus.isFailure()) { return lockStatus; }

        return saveToFiles(persistentStore(), keys);
    }

    void CSettingsCache::enableLocalSave()
    {
        connect(CSettingsCache::instance(), &CSettingsCache::valuesSaveRequested, CSettingsCache::instance(), &CSettingsCache::saveToStoreByPacket);
    }

    void CSettingsCache::saveToStoreByPacket(const CValueCachePacket &values)
    {
        QLockFile lock(lockFileName());
        const CStatusMessage lockStatus = lockFile(lock);
        if (lockStatus.isFailure())
        {
            CLogMessage::preformatted(lockStatus);
            return;
        }

        CStatusMessage status = saveToFiles(persistentStore(), values.toVariantMap());
        CLogMessage::preformatted(status);
    }

    CStatusMessage CSettingsCache::loadFromStore()
    {
        QLockFile lock(lockFileName());
        const CStatusMessage lockStatus = lockFile(lock);
        if (lockStatus.isFailure()) { return lockStatus; }

        return loadFromFiles(persistentStore());
    }

    QString CSettingsCache::filenameForKey(const QString &key)
    {
        return CFileUtils::appendFilePaths(persistentStore(), instance()->CValueCache::filenameForKey(key));
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
