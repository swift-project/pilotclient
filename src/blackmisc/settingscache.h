/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SETTINGSCACHE_H
#define BLACKMISC_SETTINGSCACHE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/valuecache.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtDebug>

namespace BlackMisc
{
    /*!
     * Singleton derived class of CValueCache, for settings.
     */
    class BLACKMISC_EXPORT CSettingsCache : public BlackMisc::CValueCache
    {
        Q_OBJECT

    public:
        //! Return the singleton instance.
        static CSettingsCache *instance();

        //! The directory where settings are stored.
        static const QString &persistentStore();

        //! Save settings to disk.
        CStatusMessage saveToStore(const QString &keyPrefix = {});

        //! Save settings to disk.
        CStatusMessage saveToStore(const QStringList &keys);

        //! Connects signal CValueCache::valuesSaveRequested to a private slot that saves the values.
        //! In a dbus distributed scenario, only call this method in the core process.
        void enableLocalSave();

        //! Load settings from disk.
        CStatusMessage loadFromStore();

        //! Return the filename where the value with the given key may be stored.
        static QString filenameForKey(const QString &key);

        //! Relative file path in application data directory
        static const QString relativeFilePath();

        //! Return all files where settings may be stored.
        QStringList enumerateStore() const;

    private:
        CSettingsCache();
        void saveToStoreByPacket(const CValueCachePacket &values);
        static const QString &lockFileName();
        CStatusMessage lockFile(QLockFile &);
    };

    /*!
     * Class template for accessing a specific value in the CSettingsCache.
     * \tparam Trait A subclass of BlackMisc::TSettingTrait that identifies the value's key and other metadata.
     */
    template <typename Trait>
    class CSetting : public BlackMisc::CCached<typename Trait::type>
    {
    public:
        //! Constructor.
        //! \param owner Will be the parent of the internal QObject used to access the value.
        template <typename T>
        CSetting(T *owner) :
            CSetting::CCached(CSettingsCache::instance(), Trait::key(), Trait::humanReadable(), Trait::isValid, Trait::defaultValue(), owner)
        {
            if (! this->isInitialized())
            {
                this->onOwnerNameChanged([this, owner] { Private::reconstruct(this, owner); });
            }
        }

        //! Constructor.
        //! \param owner Will be the parent of the internal QObject used to access the value.
        //! \param slot Slot to call when the value is modified by another object.
        //!             Must be a void, non-const member function of the owner.
        template <typename T, typename F>
        CSetting(T *owner, F slot) : CSetting(owner)
        {
            this->setNotifySlot(slot);
        }

        //! Reset the setting to its default value.
        CStatusMessage setDefault() { return this->set(Trait::defaultValue()); }

        //! Gets the default value
        auto getDefault() const { return Trait::defaultValue(); }

        //! Is default value?
        bool isDefault() const { return this->getDefault() == this->get(); }

        //! Return the file that is used for persistence for this value.
        QString getFilename() const { return CSettingsCache::filenameForKey(this->getKey()); }
    };

    /*!
     * Class template for accessing a specific value in the CSettingsCache.
     * \tparam Trait A subclass of BlackMisc::TSettingTrait that identifies the value's key and other metadata.
     */
    template <typename Trait>
    class CSettingReadOnly : public BlackMisc::CSetting<Trait>
    {
    public:
        //! Inherited constructor.
        using CSetting<Trait>::CSetting;

        //! Deleted mutators.
        //! @{
        CStatusMessage set(const typename Trait::type &value, qint64 timestamp = 0) = delete;
        CStatusMessage setAndSave(const typename Trait::type &value, qint64 timestamp = 0) = delete;
        CStatusMessage save() = delete;
        CStatusMessage setProperty(CPropertyIndexRef index, const CVariant &value, qint64 timestamp = 0) = delete;
        CStatusMessage setAndSaveProperty(CPropertyIndexRef index, const CVariant &value, qint64 timestamp = 0) = delete;
        CStatusMessage setDefault() = delete;
        //! @}
    };

    /*!
     * Base class for traits to be used as template argument to BlackMisc::CSetting.
     */
    template <typename T>
    struct TSettingTrait
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

        //! Deleted default constructor.
        TSettingTrait() = delete;

        //! Deleted copy constructor.
        TSettingTrait(const TSettingTrait &) = delete;

        //! Deleted copy assignment operator.
        TSettingTrait &operator =(const TSettingTrait &) = delete;
    };
}

#endif
