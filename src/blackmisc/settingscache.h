/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SETTINGSCACHE_H
#define BLACKMISC_SETTINGSCACHE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valuecache.h"

namespace BlackMisc
{
    /*!
     * Singleton derived class of CValueCache, for core settings.
     */
    class BLACKMISC_EXPORT CSettingsCache : public BlackMisc::CValueCache
    {
        Q_OBJECT

    public:
        //! Return the singleton instance.
        static CSettingsCache *instance();

        //! The directory where core settings are stored.
        static const QString &persistentStore();

        //! Save core settings to disk.
        BlackMisc::CStatusMessage saveToStore(const QString &keyPrefix = {});

        //! Connects signal CValueCache::valuesSaveRequested to a private slot that saves the values.
        //! In a dbus distributed scenario, only call this method in the core process.
        void enableLocalSave();

        //! Load core settings from disk.
        BlackMisc::CStatusMessage loadFromStore();

        //! Return the filename where the value with the given key may be stored.
        static QString filenameForKey(const QString &key);

        //! Return all files where settings may be stored.
        QStringList enumerateStore() const;

    private:
        CSettingsCache();
        void saveToStoreByPacket(const CValueCachePacket &values);
    };

    /*!
     * Class template for accessing a specific value in the CSettingsCache.
     * \tparam Trait A subclass of BlackMisc::CSettingTrait that identifies the value's key and other metadata.
     */
    template <typename Trait>
    class CSetting : public BlackMisc::CCached<typename Trait::type>
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
        CSetting(T *owner, NotifySlot<T> slot = nullptr) :
            CSetting::CCached(CSettingsCache::instance(), Trait::key(), Trait::isValid, Trait::defaultValue(), owner, slot)
        {}

        //! Reset the setting to its default value.
        void setDefault() { this->set(Trait::defaultValue()); }

        //! Return the file that is used for persistence for this value.
        QString getFilename() const { return CSettingsCache::filenameForKey(this->getKey()); }
    };

    /*!
     * Base class for traits to be used as template argument to BlackMisc::CSetting.
     */
    template <typename T>
    struct CSettingTrait
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
        CSettingTrait() = delete;

        //! Deleted copy constructor.
        CSettingTrait(const CSettingTrait &) = delete;

        //! Deleted copy assignment operator.
        CSettingTrait &operator =(const CSettingTrait &) = delete;
    };
}

#endif
