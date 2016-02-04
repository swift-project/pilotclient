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
#include <utility>

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

    std::future<CVariant> CDataCache::syncLoad(QObject *pageOwner, const QString &key)
    {
        auto future = m_revision.promiseLoadedValue(pageOwner, key);
        if (future.valid())
        {
            return future;
        }
        else // value is not currently loading, so immediately return the current value
        {
            std::promise<CVariant> p;
            p.set_value(getValueSync(key));
            return p.get_future();
        }
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
        auto baseline = getAllValuesWithTimestamps();
        QTimer::singleShot(0, &m_serializer, [this, baseline, values]
        {
            m_serializer.saveToStore(values.toVariantMap(), baseline);
        });
    }

    void CDataCache::loadFromStoreAsync()
    {
        auto baseline = getAllValuesWithTimestamps();
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

    void CDataCacheSerializer::saveToStore(const BlackMisc::CVariantMap &values, const BlackMisc::CValueCachePacket &baseline)
    {
        m_cache->m_revision.notifyPendingWrite();
        auto lock = loadFromStore(baseline, true); // last-minute check for remote changes before clobbering the revision file
        for (const auto &key : values.keys()) { m_deferredChanges.remove(key); } // ignore changes that we are about to overwrite

        if (! lock) { return; }
        m_cache->m_revision.writeNewRevision(baseline.toTimestampMap());

        m_cache->saveToFiles(persistentStore(), values);

        applyDeferredChanges(); // apply changes which we grabbed at the last minute above
    }

    CDataCacheRevision::LockGuard CDataCacheSerializer::loadFromStore(const BlackMisc::CValueCachePacket &baseline, bool defer)
    {
        auto lock = m_cache->m_revision.beginUpdate(baseline.toTimestampMap());
        if (lock && m_cache->m_revision.isPendingRead())
        {
            CValueCachePacket newValues;
            m_cache->loadFromFiles(persistentStore(), m_cache->m_revision.keysWithNewerTimestamps(), baseline.toVariantMap(), newValues);
            m_deferredChanges.insert(newValues);
        }

        if (! defer) { applyDeferredChanges(); }
        return lock;
    }

    void CDataCacheSerializer::applyDeferredChanges()
    {
        if (! m_deferredChanges.isEmpty())
        {
            auto promises = m_cache->m_revision.loadedValuePromises();
            for (const auto &tuple : promises)
            {
                QObject *pageOwner = nullptr;
                QString key;
                std::tie(pageOwner, key, std::ignore) = tuple;

                m_deferredChanges.inhibit(pageOwner, key); // don't fire notification slots for objects waiting on syncLoad futures
            }

            m_deferredChanges.setSaved();
            emit valuesLoadedFromStore(m_deferredChanges, CIdentifier::anonymous());
            deliverPromises(std::move(promises));
            m_deferredChanges.clear();
        }
    }

    void CDataCacheSerializer::deliverPromises(std::vector<std::tuple<QObject *, QString, std::promise<CVariant>>> i_promises)
    {
        auto changes = m_deferredChanges;
        auto promises = std::make_shared<decltype(i_promises)>(std::move(i_promises)); // \todo use C++14 lambda init-capture
        QTimer::singleShot(0, Qt::PreciseTimer, this, [this, changes, promises]
        {
            for (auto &tuple : *promises)
            {
                QString key;
                std::promise<CVariant> promise;
                std::tie(std::ignore, key, promise) = std::move(tuple);

                promise.set_value(changes.value(key).first);
            }
        });
    }

    CDataCacheRevision::LockGuard CDataCacheRevision::beginUpdate(const QMap<QString, qint64> &timestamps)
    {
        QMutexLocker lock(&m_mutex);

        Q_ASSERT(! m_updateInProgress);
        Q_ASSERT(! m_lockFile.isLocked());
        Q_ASSERT(m_promises.empty());

        if (! m_lockFile.lock())
        {
            CLogMessage(this).error("Failed to lock %1: %2") << m_basename << lockFileError(m_lockFile);
            return {};
        }
        m_updateInProgress = true;
        LockGuard guard(this);

        m_timestamps.clear();

        QFile revisionFile(m_basename + "/.rev");
        if (revisionFile.exists())
        {
            if (! revisionFile.open(QFile::ReadOnly))
            {
                CLogMessage(this).error("Failed to open %1: %2") << revisionFile.fileName() << revisionFile.errorString();
                return {};
            }

            auto json = QJsonDocument::fromJson(revisionFile.readAll()).object();
            if (json.contains("uuid") && json.contains("timestamps"))
            {
                QUuid uuid(json.value("uuid").toString());
                if (uuid == m_uuid)
                {
                    if (m_pendingWrite) { return guard; }
                    return {};
                }
                m_uuid = uuid;

                auto newTimestamps = fromJson(json.value("timestamps").toObject());
                for (auto it = newTimestamps.cbegin(); it != newTimestamps.cend(); ++it)
                {
                    if (timestamps.value(it.key(), 0) < it.value())
                    {
                        m_timestamps.insert(it.key(), it.value());
                    }
                }
                if (m_timestamps.isEmpty())
                {
                    if (m_pendingWrite) { return guard; }
                    return {};
                }
            }
            else if (revisionFile.size() > 0)
            {
                CLogMessage(this).error("Invalid format of %1") << revisionFile.fileName();

                if (m_pendingWrite) { return guard; }
                return {};
            }
        }

        m_pendingRead = true;
        return guard;
    }

    void CDataCacheRevision::writeNewRevision(const QMap<QString, qint64> &i_timestamps)
    {
        QMutexLocker lock(&m_mutex);

        Q_ASSERT(m_updateInProgress);
        Q_ASSERT(m_pendingWrite);
        Q_ASSERT(m_lockFile.isLocked());

        QFile revisionFile(m_basename + "/.rev");
        if (! revisionFile.open(QFile::WriteOnly))
        {
            CLogMessage(this).error("Failed to open %1: %2") << revisionFile.fileName() << revisionFile.errorString();
            return;
        }

        m_uuid = CIdentifier().toUuid();
        auto timestamps = m_timestamps;
        for (auto it = i_timestamps.cbegin(); it != i_timestamps.cend(); ++it)
        {
            timestamps.insert(it.key(), it.value());
        }

        QJsonObject json;
        json.insert("uuid", m_uuid.toString());
        json.insert("timestamps", toJson(timestamps));
        revisionFile.write(QJsonDocument(json).toJson());
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

    QSet<QString> CDataCacheRevision::keysWithNewerTimestamps() const
    {
        QMutexLocker lock(&m_mutex);

        Q_ASSERT(m_updateInProgress);
        return QSet<QString>::fromList(m_timestamps.keys());
    }

    bool CDataCacheRevision::isNewerValueAvailable(const QString &key) const
    {
        QMutexLocker lock(&m_mutex);

        return m_updateInProgress && m_timestamps.contains(key);
    }

    std::future<CVariant> CDataCacheRevision::promiseLoadedValue(QObject *pageOwner, const QString &key)
    {
        QMutexLocker lock(&m_mutex);

        if (isNewerValueAvailable(key))
        {
            std::promise<CVariant> promise;
            auto future = promise.get_future();
            m_promises.emplace_back(pageOwner, key, std::move(promise));
            return future;
        }
        return {};
    }

    std::vector<std::tuple<QObject *, QString, std::promise<CVariant>>> CDataCacheRevision::loadedValuePromises()
    {
        QMutexLocker lock(&m_mutex);

        Q_ASSERT(m_updateInProgress);
        return std::move(m_promises); // move into the return value, so m_promises becomes empty
    }

    QJsonObject CDataCacheRevision::toJson(const QMap<QString, qint64> &timestamps)
    {
        QJsonObject result;
        for (auto it = timestamps.begin(); it != timestamps.end(); ++it)
        {
            result.insert(it.key(), it.value());
        }
        return result;
    }

    QMap<QString, qint64> CDataCacheRevision::fromJson(const QJsonObject &timestamps)
    {
        QMap<QString, qint64> result;
        for (auto it = timestamps.begin(); it != timestamps.end(); ++it)
        {
            result.insert(it.key(), static_cast<qint64>(it.value().toDouble()));
        }
        return result;
    }
}
