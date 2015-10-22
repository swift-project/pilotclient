/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DATACACHE_H
#define BLACKCORE_DATACACHE_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/valuecache.h"
#include <QUuid>
#include <QFileSystemWatcher>

namespace BlackCore
{

    /*!
     * Singleton derived class of CValueCache, for core dynamic data.
     *
     * File-based distribution between processes is built-in to the class.
     */
    class BLACKCORE_EXPORT CDataCache : public BlackMisc::CValueCache
    {
    public:
        //! Return the singleton instance.
        static CDataCache *instance();

        //! The directory where core data are stored.
        static const QString &persistentStore();

    private:
        CDataCache();

        //! Save values to persistent store. Called whenever a value is changed locally.
        void saveToStore(const BlackMisc::CVariantMap &values);

        //! Load values from persistent store. Called once per second.
        //! Also called by saveToStore, to ensure that remote changes to unrelated values are not lost.
        //! \param lock Whether to acquire the revision file lock. Used when called by saveToStore.
        //! \param defer Whether to defer applying the changes. Used when called by saveToStore.
        void loadFromStore(bool lock = true, bool defer = false);

        QFileSystemWatcher m_watcher;
        QUuid m_revision;
        const QString m_revisionFileName { persistentStore() + "/.rev" };
        BlackMisc::CValueCachePacket m_deferredChanges;
    };

    /*!
     * Class template for accessing a specific value in the CDataCache.
     * \tparam Trait A subclass of BlackCore::CDataTrait that identifies the value's key and other metadata.
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
    };

    /*!
     * Base class for traits to be used as template argument to BlackCore::CData.
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
    };

}

#endif
