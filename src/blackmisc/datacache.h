/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DATACACHE_H
#define BLACKMISC_DATACACHE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valuecache.h"
#include "blackmisc/worker.h"
#include <QUuid>
#include <QFileSystemWatcher>

namespace BlackMisc
{
    class CDataCache;

    /*!
     * Worker which performs (de)serialization on behalf of CDataCache, in a separate thread
     * so that the main thread is not blocked by (de)serialization of large objects.
     */
    class BLACKMISC_EXPORT CDataCacheSerializer : public BlackMisc::CContinuousWorker
    {
        Q_OBJECT

    public:
        //! Constructor.
        CDataCacheSerializer(CDataCache *owner, const QString &revisionFileName);

        //! Save values to persistent store. Called whenever a value is changed locally.
        void saveToStore(const BlackMisc::CVariantMap &values, const BlackMisc::CVariantMap &baseline);

        //! Load values from persistent store. Called once per second.
        //! Also called by saveToStore, to ensure that remote changes to unrelated values are not lost.
        //! \param baseline A snapshot of the currently loaded values, taken when the load is queued.
        //! \param lock Whether to acquire the revision file lock. Used when called by saveToStore.
        //! \param defer Whether to defer applying the changes. Used when called by saveToStore.
        void loadFromStore(const BlackMisc::CVariantMap &baseline, bool lock = true, bool defer = false);

    signals:
        //! Signal back to the cache when values have been loaded.
        void valuesLoadedFromStore(const BlackMisc::CValueCachePacket &values, const BlackMisc::CIdentifier &originator);

    private:
        const QString &persistentStore() const;

        const CDataCache *const m_cache = nullptr;
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
        //! Return the singleton instance.
        static CDataCache *instance();

        //! The directory where core data are stored.
        static const QString &persistentStore();

        //! Return the filename where the value with the given key may be stored.
        static QString filenameForKey(const QString &key);

        //! Return all files where data may be stored.
        QStringList enumerateStore() const;

    private:
        CDataCache();

        void saveToStoreAsync(const BlackMisc::CValueCachePacket &values);
        void loadFromStoreAsync();

        QFileSystemWatcher m_watcher;
        const QString m_revisionFileName { persistentStore() + "/.rev" };

        CDataCacheSerializer m_serializer { this, m_revisionFileName };
        friend class CDataCacheSerializer; // to access protected members of CValueCache
    };

    /*!
     * Class template for accessing a specific value in the CDataCache.
     * \tparam Trait A subclass of BlackMisc::CDataTrait that identifies the value's key and other metadata.
     */
    template <typename Trait>
    class CData : public BlackMisc::CCached<typename Trait::type>
    {
    public:
        //! \copydoc BlackMisc::CCached::NotifySlot
        template <typename T>
        using NotifySlot = typename BlackMisc::CCached<typename Trait::type>::template NotifySlot<T>;

        //! Constructor.
        //! \param owner Will be the parent of the internal QObject used to access the value.
        //! \param slot Slot to call when the value is modified by another object.
        //!             Must be a void, non-const member function of the owner.
        template <typename T>
        CData(T *owner, NotifySlot<T> slot = nullptr) :
            CData::CCached(CDataCache::instance(), Trait::key(), Trait::isValid, Trait::defaultValue(), owner, slot)
        {}

        //! Reset the data to its default value.
        void setDefault() { this->set(Trait::defaultValue()); }

        //! Return the file that is used for persistence for this value.
        QString getFilename() const { return CDataCache::filenameForKey(this->getKey()); }
    };

    /*!
     * Base class for traits to be used as template argument to BlackMisc::CData.
     */
    template <typename T>
    struct CDataTrait
    {
        //! Data type of the value.
        using type = T;

        //! Key string of the value. Reimplemented in derived class.
        static const char *key() { qFatal("Not implemented"); return ""; }

        //! Validator function. Return true if the argument is valid, false otherwise. Default
        //! implementation just returns true. Reimplemented in derived class to support validation of the value.
        static bool isValid(const T &) { return true; }

        //! Return the value to use in case the supplied value does not satisfy the validator.
        //! Default implementation returns a default-constructed value.
        static const T &defaultValue() { static const T def {}; return def; }

        //! Deleted default constructor.
        CDataTrait() = delete;

        //! Deleted copy constructor.
        CDataTrait(const CDataTrait &) = delete;

        //! Deleted copy assignment operator.
        CDataTrait &operator =(const CDataTrait &) = delete;

        //! Data cache doesn't support setAndSave (because set() already causes save anyway).
        CStatusMessage setAndSave(const T &value) = delete;
    };
}

#endif
