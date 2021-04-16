/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DATACACHE_H
#define BLACKMISC_DATACACHE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/identifier.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/valuecache.h"
#include "blackmisc/valuecacheprivate.h"
#include "blackmisc/variantmap.h"
#include "blackmisc/worker.h"

#include <QDateTime>
#include <QFileSystemWatcher>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QLockFile>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QSet>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QUuid>
#include <QtDebug>
#include <QtGlobal>
#include <future>
#include <memory>
#include <utility>
#include <vector>

namespace BlackMisc
{
    namespace Private
    {
        /*!
         * Decorator for CValuePage which allows incoming remote changes to be queued to allow for more
         * flexibility to control how the queue is popped compared to the usual Qt::QueuedConnection.
         */
        class BLACKMISC_EXPORT CDataPageQueue : public QObject
        {
            Q_OBJECT

        public:
            //! Constructor.
            CDataPageQueue(CValuePage *parent) : QObject(parent), m_page(parent) {}

            //! Add to the queue to synchronize with a change caused by another page.
            //! \threadsafe
            void queueValuesFromCache(const BlackMisc::CValueCachePacket &values, QObject *changedBy);

            //! Synchronize with changes queued by queueValuesFromCache.
            void setQueuedValuesFromCache();

            //! Synchronize with one specific change in the queue, leave the rest for later.
            void setQueuedValueFromCache(const QString &key);

        private:
            CValuePage *m_page = nullptr;
            QList<std::pair<CValueCachePacket, QObject *>> m_queue;
            QMutex m_mutex;
        };
    }

    class CDataCache;

    /*!
     * Encapsulates metastate about how the version of the cache in memory compares to the one on disk.
     * \threadsafe
     */
    class BLACKMISC_EXPORT CDataCacheRevision
    {
    public:
        //! Construct the single instance of the revision metastate.
        CDataCacheRevision(const QString &basename);

        //! Destructor.
        ~CDataCacheRevision();

        //! Non-copyable.
        //! @{
        CDataCacheRevision(const CDataCacheRevision &) = delete;
        CDataCacheRevision &operator =(const CDataCacheRevision &) = delete;
        //! @}

        //! RAII class to keep the revision file locked during update.
        class LockGuard;

        //! Get the state of the disk cache, and prepare to update any values which are out of date.
        //! Return value can be converted to bool, false means update is not started (error, or already up-to-date).
        //! \param timestamps Current in-memory timestamps, to be compared with the on-disk ones.
        //! \param updateUuid Whether to prepare for an actual update, or just interrograte whether one is needed.
        //! \param pinsOnly Only load pinned values.
        LockGuard beginUpdate(const QMap<QString, qint64> &timestamps, bool updateUuid = true, bool pinsOnly = false);

        //! During update, writes a new revision file with new timestamps.
        void writeNewRevision(const QMap<QString, qint64> &timestamps, const QSet<QString> &excludeKeys = {});

        //! Write a new revision file with keys deduced from the available JSON files.
        void regenerate(const CValueCachePacket &keys);

        //! Release the revision file lock and mark everything up-to-date (called by LockGuard destructor).
        //! \param keepPromises Don't break pending promises.
        void finishUpdate(bool keepPromises = false);

        //! Existing revision file was found.
        bool isFound() const;

        //! True if beginUpdate found some values with timestamps newer than in memory.
        bool isPendingRead() const;

        //! Call before beginUpdate if there is a write pending, so update will start even if there is nothing to read.
        void notifyPendingWrite();

        //! During update, returns keys which have on-disk timestamps newer than in-memory. Guaranteed not empty.
        QSet<QString> keysWithNewerTimestamps() const;

        //! During update, returns timestamps which have on-disk timestamps newer than in-memory. Guaranteed not empty.
        const QMap<QString, qint64> &newerTimestamps() const;

        //! During update, returns true if the on-disk timestamp of this key is newer than in-memory.
        bool isNewerValueAvailable(const QString &key, qint64 timestamp);

        //! Return a future which will be made ready when the value is loaded. Future is invalid if value is not loading.
        std::future<void> promiseLoadedValue(const QString &key, qint64 currentTimestamp);

        //! Returns (by move) the container of promises to load values.
        std::vector<std::promise<void>> loadedValuePromises();

        //! Abandon all promises.
        void breakPromises();

        //! Keys with timestamps.
        QString timestampsAsString() const;

        //! Set TTL value that will be written to the revision file.
        void setTimeToLive(const QString &key, int ttl);

        //! Causes the new timestamp to be written to the revision file.
        void overrideTimestamp(const QString &key, qint64 timestamp);

        //! Read the revision file to get a timestamp.
        qint64 getTimestampOnDisk(const QString &key);

        //! Set the flag which will cause the value to be pre-loaded.
        void pinValue(const QString &key);

        //! Set the flag which will cause the value to be deferred-loaded.
        void deferValue(const QString &key);

        //! Set the flag which will cause a deferred-load value to be loaded.
        void admitValue(const QString &key);

        //! Set the flag which will cause a value to be reset when starting a new session.
        void sessionValue(const QString &key);

    private:
        mutable QMutex m_mutex { QMutex::Recursive };
        bool m_updateInProgress = false;
        bool m_found = false;
        bool m_pendingRead = false;
        bool m_pendingWrite = false;
        QString m_basename;
        QLockFile m_lockFile { m_basename + "/.lock" };
        QUuid m_uuid;
        QMap<QString, qint64> m_timestamps;
        QMap<QString, qint64> m_originalTimestamps;
        QMap<QString, qint64> m_timesToLive;
        QSet<QString> m_pinnedValues;
        QSet<QString> m_deferredValues;
        QSet<QString> m_admittedValues;
        QSet<QString> m_admittedQueue;
        QMap<QString, QUuid> m_sessionValues;
        std::vector<std::promise<void>> m_promises;

        class Session;
        std::unique_ptr<Session> m_session;

        static QJsonObject toJson(const QMap<QString, qint64> &timestamps);
        static QMap<QString, qint64> fromJson(const QJsonObject &timestamps);
        static QJsonArray toJson(const QSet<QString> &pins);
        static QSet<QString> fromJson(const QJsonArray &pins);
        static QJsonObject toJson(const QMap<QString, QUuid> &session);
        static QMap<QString, QUuid> sessionFromJson(const QJsonObject &session);
    };

    /*!
     * Worker which performs (de)serialization on behalf of CDataCache, in a separate thread
     * so that the main thread is not blocked by (de)serialization of large objects.
     */
    class BLACKMISC_EXPORT CDataCacheSerializer : public BlackMisc::CContinuousWorker
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor.
        CDataCacheSerializer(CDataCache *owner, const QString &revisionFileName);

        //! Save values to persistent store. Called whenever a value is changed locally.
        void saveToStore(const BlackMisc::CVariantMap &values, const BlackMisc::CValueCachePacket &baseline);

        //! Load values from persistent store. Called once per second.
        //! Also called by saveToStore, to ensure that remote changes to unrelated values are not lost.
        //! \param baseline A snapshot of the currently loaded values, taken when the load is queued.
        //! \param defer Whether to defer applying the changes. Used when called by saveToStore.
        //! \param pinsOnly Only load pinned values.
        //! \return Usually ignored, but can be held in order to retain the revision file lock.
        CDataCacheRevision::LockGuard loadFromStore(const BlackMisc::CValueCachePacket &baseline, bool defer = false, bool pinsOnly = false);

    signals:
        //! Signal back to the cache when values have been loaded.
        void valuesLoadedFromStore(const BlackMisc::CValueCachePacket &values, const BlackMisc::CIdentifier &originator);

    private:
        const QString &persistentStore() const;
        void applyDeferredChanges();
        void deliverPromises(std::vector<std::promise<void>>);

        CDataCache *const m_cache = nullptr;
        QUuid m_revision;
        const QString m_revisionFileName;
        BlackMisc::CValueCachePacket m_deferredChanges;
    };

    /*!
     * Singleton derived class of CValueCache, for core dynamic data.
     *
     * File-based distribution between processes is built-in to the class.
     */
    class BLACKMISC_EXPORT CDataCache : public BlackMisc::CValueCache
    {
        Q_OBJECT

    public:
        //! Destructor.
        virtual ~CDataCache() override;

        //! Return the singleton instance.
        static CDataCache *instance();

        //! The directory where core data are stored.
        static const QString &persistentStore();

        //! Revision file name
        static const QString &revisionFileName();

        //! Return the filename where the value with the given key may be stored.
        static QString filenameForKey(const QString &key);

        //! Return all files where data may be stored.
        QStringList enumerateStore() const;

        //! Method used for implementing CData::synchronize.
        bool synchronize(const QString &key);

        //! Method used for implementing TTL.
        void setTimeToLive(const QString &key, int ttl);

        //! Method used for implementing timestamp renewal.
        void renewTimestamp(const QString &key, qint64 timestamp);

        //! Method used for implementing loading timestamp without value.
        qint64 getTimestampOnDisk(const QString &key);

        //! Method used for implementing pinning values.
        void pinValue(const QString &key);

        //! Method used for implementing deferring values.
        void deferValue(const QString &key);

        //! Method used for implementing deferring values.
        void admitValue(const QString &key, bool triggerLoad);

        //! Method used for implementing session values.
        void sessionValue(const QString &key);

        //! Relative file path in application data directory
        static const QString relativeFilePath();

    private:
        CDataCache();

        void saveToStoreAsync(const BlackMisc::CValueCachePacket &values);
        void loadFromStoreAsync();

        virtual void connectPage(Private::CValuePage *page) override;

        QFileSystemWatcher m_watcher;

        CDataCacheSerializer *m_serializer = nullptr;
        CDataCacheRevision m_revision { persistentStore() + "/" };
        friend class CDataCacheSerializer; // to access m_revision and protected members of CValueCache
    };

    /*!
     * Class template for accessing a specific value in the CDataCache.
     * \tparam Trait A subclass of BlackMisc::TDataTrait that identifies the value's key and other metadata.
     */
    template <typename Trait>
    class CData : public CCached<typename Trait::type>
    {
    public:
        //! Constructor.
        //! \param owner Will be the parent of the internal QObject used to access the value.
        template <typename T>
        CData(T *owner) :
            CData::CCached(CDataCache::instance(), Trait::key(), Trait::humanReadable(), Trait::isValid, Trait::defaultValue(), owner)
        {
            if (! this->isInitialized())
            {
                this->onOwnerNameChanged([this, owner] { Private::reconstruct(this, owner); });
                return;
            }
            if (Trait::timeToLive() >= 0) { CDataCache::instance()->setTimeToLive(this->getKey(), Trait::timeToLive()); }
            if (Trait::isPinned())   { CDataCache::instance()->pinValue(this->getKey()); }
            if (Trait::isDeferred()) { CDataCache::instance()->deferValue(this->getKey()); }
            if (Trait::isSession())  { CDataCache::instance()->sessionValue(this->getKey()); }
            static_assert(!(Trait::isPinned() && Trait::isDeferred()), "trait can not be both pinned and deferred");
        }

        //! Constructor.
        //! \param owner Will be the parent of the internal QObject used to access the value.
        //! \param slot Slot to call when the value is modified by another object.
        //!             Must be a void, non-const member function of the owner.
        template <typename T, typename F>
        CData(T *owner, F slot) : CData(owner)
        {
            this->setNotifySlot(slot);
        }

        //! \copydoc BlackMisc::CCached::set
        CStatusMessage set(const typename Trait::type &value, qint64 timestamp = 0)
        {
            CDataCache::instance()->admitValue(this->getKey(), false);
            return CCached<typename Trait::type>::set(value, timestamp);
        }

        //! \copydoc BlackMisc::CCached::setProperty
        CStatusMessage setProperty(CPropertyIndexRef index, const CVariant &value, qint64 timestamp = 0)
        {
            CDataCache::instance()->admitValue(this->getKey(), false);
            return CCached<typename Trait::type>::setProperty(index, value, timestamp);
        }

        //! Reset the data to its default value.
        CStatusMessage setDefault() { return this->set(Trait::defaultValue()); }

        //! Return the file that is used for persistence for this value.
        QString getFilename() const { return CDataCache::filenameForKey(this->getKey()); }

        //! True if the current timestamp is older than the TTL (time to live).
        bool isStale() const { return Trait::timeToLive() >= 0 && this->getTimestamp() + Trait::timeToLive() > QDateTime::currentMSecsSinceEpoch(); }

        //! Don't change the value, but write a new timestamp, to extend the life of the value.
        void renewTimestamp(qint64 timestamp) { return CDataCache::instance()->renewTimestamp(this->getKey(), timestamp); }

        //! Get the timestamp of the value, or of the deferred value that is available to be loaded.
        QDateTime getAvailableTimestamp() const
        {
            if (Trait::isDeferred()) { return QDateTime::fromMSecsSinceEpoch(CDataCache::instance()->getTimestampOnDisk(this->getKey())); }
            return this->getTimestamp();
        }

        //! If the value is load-deferred, trigger the deferred load (async).
        void admit() { if (Trait::isDeferred()) { CDataCache::instance()->admitValue(this->getKey(), true); } }

        //! If the value is currently being loaded, wait for it to finish loading, and call the notification slot, if any.
        void synchronize()
        {
            auto *queue = this->m_page->template findChild<Private::CDataPageQueue *>();
            Q_ASSERT(queue);
            this->admit();
            const QString key(this->getKey());
            CDataCache::instance()->synchronize(key);
            CDataCache::instance()->synchronize(key); // if load was in progress when admit() was called, synchronize with the next load

            // run in page thread
            //! \todo KB 2018-01 is this OK or should it go to CValuePage::setValuesFromCache?
            if (CThreadUtils::isInThisThread(this->m_page)) { queue->setQueuedValueFromCache(key); }
            else
            {
                QPointer<QObject> myself(queue);
                QTimer::singleShot(0, queue, [ = ]
                {
                    if (!myself) { return; }
                    queue->setQueuedValueFromCache(key);
                });
            }
        }

        //! Data cache doesn't support setAndSave (because set() already causes save anyway).
        //! @{
        CStatusMessage setAndSave(const typename Trait::type &value, qint64 timestamp = 0) = delete;
        CStatusMessage setAndSaveProperty(CPropertyIndexRef index, const CVariant &value, qint64 timestamp = 0) = delete;
        //! @}

        //! Data cache doesn't support save (because currently set value is saved already).
        CStatusMessage save() = delete;
    };

    /*!
     * Class template for read-only access to a specific value in the CDataCache.
     * \tparam Trait A subclass of BlackMisc::TDataTrait that identifies the value's key and other metadata.
     */
    template <typename Trait>
    class CDataReadOnly : public BlackMisc::CData<Trait>
    {
    public:
        //! Inherited constructor.
        using CData<Trait>::CData;

        //! Deleted mutators.
        //! @{
        CStatusMessage set(const typename Trait::type &value, qint64 timestamp = 0) = delete;
        CStatusMessage setProperty(CPropertyIndexRef index, const CVariant &value, qint64 timestamp = 0) = delete;
        CStatusMessage setDefault() = delete;
        void renewTimestamp(qint64 timestamp) = delete;
        //! @}
    };

    /*!
     * Base class for traits to be used as template argument to BlackMisc::CData.
     */
    template <typename T>
    struct TDataTrait
    {
        //! Data type of the value.
        using type = T;

        //! Key string of the value. Reimplemented in derived class.
        static const char *key() { qFatal("Not implemented"); return ""; }

        //! Optional human readable name.
        static const QString &humanReadable() { static const QString name; return name; }

        //! Validator function. Return true if the argument is valid, false otherwise. Default
        //! implementation just returns true. Reimplemented in derived class to support validation of the value.
        static bool isValid(const T &value, QString &reason) { Q_UNUSED(value); Q_UNUSED(reason); return true; }

        //! Return the value to use in case the supplied value does not satisfy the validator.
        //! Default implementation returns a default-constructed value.
        static const T &defaultValue() { static const T def {}; return def; }

        //! Number of milliseconds after which cached value becomes stale.
        //! Default is -1 which means value never becomes stale.
        static int timeToLive() { return -1; }

        //! If true, then value will be synchronously loaded when CDataCache is constructed.
        //! Good for small, important values; bad for large ones.
        static constexpr bool isPinned() { return false; }

        //! If true, then value will not be loaded until it is explicitly admitted.
        //! Good for large values the loading of which might depend on some other condition.
        static constexpr bool isDeferred() { return false; }

        //! If true, then upon starting an application, value will be overwritten with the default
        //! if there are no other applications currently using the cache. In effect, the value
        //! is retained only while there are applications using the cache.
        static constexpr bool isSession() { return false; }

        //! Deleted default constructor.
        TDataTrait() = delete;

        //! Deleted copy constructor.
        TDataTrait(const TDataTrait &) = delete;

        //! Deleted copy assignment operator.
        TDataTrait &operator =(const TDataTrait &) = delete;
    };
}

#endif
