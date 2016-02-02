/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "datacache.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/identifier.h"
#include <QStandardPaths>

namespace BlackMisc
{
    class CDataCacheRevision::LockGuard
    {
    public:
        LockGuard(const LockGuard &) = delete;
        LockGuard &operator =(const LockGuard &) = delete;
        LockGuard(LockGuard &&other) : m_movedFrom(true) { *this = std::move(other); }
        LockGuard &operator =(LockGuard &&other) { std::swap(m_movedFrom, other.m_movedFrom); std::swap(m_rev, other.m_rev); return *this; }

        ~LockGuard()
        {
            if (! m_movedFrom) { m_rev->finishUpdate(); }
        }

        operator bool() const { return ! m_movedFrom; }

    private:
        LockGuard() : m_movedFrom(true) {}
        LockGuard(CDataCacheRevision *rev) : m_movedFrom(! rev), m_rev(rev) {}
        friend class CDataCacheRevision;

        bool m_movedFrom = false;
        CDataCacheRevision *m_rev = nullptr;
    };

    CDataCache::CDataCache() :
        CValueCache(CValueCache::Distributed)
    {
        if (! QDir::root().mkpath(persistentStore()))
        {
            CLogMessage(this).error("Failed to create directory %1") << persistentStore();
        }

        connect(this, &CValueCache::valuesChangedByLocal, this, &CDataCache::saveToStoreAsync);
        connect(this, &CValueCache::valuesChangedByLocal, this, [ = ](CValueCachePacket values)
        {
            values.setSaved();
            changeValuesFromRemote(values, CIdentifier());
        });
        connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &CDataCache::loadFromStoreAsync);
        connect(&m_serializer, &CDataCacheSerializer::valuesLoadedFromStore, this, &CDataCache::changeValuesFromRemote);

        if (! QFile::exists(m_revisionFileName)) { QFile(m_revisionFileName).open(QFile::WriteOnly); }
        m_watcher.addPath(m_revisionFileName);
        m_serializer.start();
        loadFromStoreAsync();
    }

    CDataCache *CDataCache::instance()
    {
        static CDataCache cache;
        return &cache;
    }

    const QString &CDataCache::persistentStore()
    {
        static const QString dir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/org.swift-project/data/cache/core";
        return dir;
    }

    QString CDataCache::filenameForKey(const QString &key)
    {
        return persistentStore() + "/" + CValueCache::filenameForKey(key);
    }

    QStringList CDataCache::enumerateStore() const
    {
        return enumerateFiles(persistentStore());
    }

    QString lockFileError(const QLockFile &lock)
    {
        switch (lock.error())
        {
        case QLockFile::NoError: return "No error";
        case QLockFile::PermissionError: return "Insufficient permission";
        case QLockFile::UnknownError: return "Unknown filesystem error";
        case QLockFile::LockFailedError:
        {
            QString hostname, appname;
            qint64 pid = 0;
            lock.getLockInfo(&pid, &hostname, &appname);
            return QString("Lock open in another process (%1 %2 on %3)").arg(hostname, QString::number(pid), appname);
        }
        default: return "Bad error number";
        }
    }

    void CDataCache::saveToStoreAsync(const BlackMisc::CValueCachePacket &values)
    {
        auto baseline = getAllValues();
        QTimer::singleShot(0, &m_serializer, [this, baseline, values]
        {
            m_serializer.saveToStore(values.toVariantMap(), baseline);
        });
    }

    void CDataCache::loadFromStoreAsync()
    {
        auto baseline = getAllValues();
        QTimer::singleShot(0, &m_serializer, [this, baseline]
        {
            m_serializer.loadFromStore(baseline);
        });
    }

    CDataCacheSerializer::CDataCacheSerializer(CDataCache *owner, const QString &revisionFileName) :
        CContinuousWorker(owner),
        m_cache(owner),
        m_revisionFileName(revisionFileName)
    {}

    const QString &CDataCacheSerializer::persistentStore() const
    {
        return m_cache->persistentStore();
    }

    void CDataCacheSerializer::saveToStore(const BlackMisc::CVariantMap &values, const BlackMisc::CVariantMap &baseline)
    {
        m_cache->m_revision.notifyPendingWrite();
        auto lock = loadFromStore(baseline, true); // last-minute check for remote changes before clobbering the revision file
        for (const auto &key : values.keys()) { m_deferredChanges.remove(key); } // ignore changes that we are about to overwrite

        if (! lock) { return; }
        m_cache->m_revision.writeNewRevision();
        m_cache->saveToFiles(persistentStore(), values);

        if (! m_deferredChanges.isEmpty()) // apply changes which we grabbed at the last minute above
        {
            m_deferredChanges.setSaved();
            emit valuesLoadedFromStore(m_deferredChanges, CIdentifier::anonymous());
            m_deferredChanges.clear();
        }
    }

    CDataCacheRevision::LockGuard CDataCacheSerializer::loadFromStore(const BlackMisc::CVariantMap &baseline, bool defer)
    {
        auto lock = m_cache->m_revision.beginUpdate();
        if (lock && m_cache->m_revision.isPendingRead())
        {
            CValueCachePacket newValues;
            m_cache->loadFromFiles(persistentStore(), baseline, newValues);
            m_deferredChanges.insert(newValues);
        }

        if (! (m_deferredChanges.isEmpty() || defer))
        {
            m_deferredChanges.setSaved();
            emit valuesLoadedFromStore(m_deferredChanges, CIdentifier::anonymous());
            m_deferredChanges.clear();
        }
        return lock;
    }

    CDataCacheRevision::LockGuard CDataCacheRevision::beginUpdate()
    {
        QMutexLocker lock(&m_mutex);

        Q_ASSERT(! m_updateInProgress);
        Q_ASSERT(! m_lockFile.isLocked());

        if (! m_lockFile.lock())
        {
            CLogMessage(this).error("Failed to lock %1: %2") << m_basename << lockFileError(m_lockFile);
            return {};
        }
        m_updateInProgress = true;
        LockGuard guard(this);

        QFile revisionFile(m_basename + "/.rev");
        if (revisionFile.exists())
        {
            if (! revisionFile.open(QFile::ReadOnly))
            {
                CLogMessage(this).error("Failed to open %1: %2") << revisionFile.fileName() << revisionFile.errorString();
                return {};
            }

            QUuid uuid(revisionFile.readAll());
            if (uuid == m_uuid)
            {
                if (m_pendingWrite) { return guard; }
                return {};
            }
        }

        m_pendingRead = true;
        return guard;
    }

    void CDataCacheRevision::writeNewRevision()
    {
        QMutexLocker lock(&m_mutex);

        Q_ASSERT(m_updateInProgress);
        Q_ASSERT(m_lockFile.isLocked());

        QFile revisionFile(m_basename + "/.rev");
        if (! revisionFile.open(QFile::WriteOnly))
        {
            CLogMessage(this).error("Failed to open %1: %2") << revisionFile.fileName() << revisionFile.errorString();
            return;
        }

        m_uuid = CIdentifier().toUuid();
        revisionFile.write(m_uuid.toByteArray());
    }

    void CDataCacheRevision::finishUpdate()
    {
        QMutexLocker lock(&m_mutex);

        Q_ASSERT(m_updateInProgress);
        Q_ASSERT(m_lockFile.isLocked());

        m_updateInProgress = false;
        m_pendingRead = false;
        m_pendingWrite = false;
        m_lockFile.unlock();
    }

    bool CDataCacheRevision::isPendingRead() const
    {
        QMutexLocker lock(&m_mutex);

        Q_ASSERT(m_updateInProgress);
        return ! m_timestamps.isEmpty();
    }

    void CDataCacheRevision::notifyPendingWrite()
    {
        m_pendingWrite = true;
    }
}
