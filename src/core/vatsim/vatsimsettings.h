// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_VATSIM_SETTINGS_H
#define SWIFT_CORE_VATSIM_SETTINGS_H

#include "core/swiftcoreexport.h"
#include "misc/directoryutils.h"
#include "misc/network/serverlist.h"
#include "misc/pq/time.h"
#include "misc/settingscache.h"
#include "misc/swiftdirectories.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::core::vatsim, CReaderSettings)
SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::core::vatsim, CRawFsdMessageSettings)

namespace swift::core::vatsim
{
    /*!
     * Settings used with readers
     */
    class SWIFT_CORE_EXPORT CReaderSettings : public swift::misc::CValueObject<swift::core::vatsim::CReaderSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexInitialTime = swift::misc::CPropertyIndexRef::GlobalIndexCSettingsReaders,
            IndexPeriodicTime,
            IndexNeverUpdate
        };

        //! Default constructor.
        CReaderSettings();

        //! Simplified constructor
        CReaderSettings(const swift::misc::physical_quantities::CTime &initialTime,
                        const swift::misc::physical_quantities::CTime &periodicTime, bool neverUpdate = false);

        //! Get time
        const swift::misc::physical_quantities::CTime &getInitialTime() const { return m_initialTime; }

        //! Set time
        void setInitialTime(const swift::misc::physical_quantities::CTime &time) { m_initialTime = time; }

        //! Get time
        const swift::misc::physical_quantities::CTime &getPeriodicTime() const { return m_periodicTime; }

        //! Set time
        void setPeriodicTime(const swift::misc::physical_quantities::CTime &time) { m_periodicTime = time; }

        //! Never ever update?
        bool isNeverUpdate() const { return m_neverUpdate; }

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Settings used when a reader is manually triggered and never updates
        static const CReaderSettings &neverUpdateSettings();

    private:
        swift::misc::physical_quantities::CTime m_initialTime { 30.0,
                                                                swift::misc::physical_quantities::CTimeUnit::s() };
        swift::misc::physical_quantities::CTime m_periodicTime { 30.0,
                                                                 swift::misc::physical_quantities::CTimeUnit::s() };
        bool m_neverUpdate = false;

        SWIFT_METACLASS(
            CReaderSettings,
            SWIFT_METAMEMBER(initialTime),
            SWIFT_METAMEMBER(periodicTime),
            SWIFT_METAMEMBER(neverUpdate));
    };

    //! Reader settings
    struct TVatsimDataFile : public swift::misc::TSettingTrait<CReaderSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "vatsimreaders/datafile"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("VATSIM data file");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const swift::core::vatsim::CReaderSettings &defaultValue()
        {
            static const swift::core::vatsim::CReaderSettings reader {
                { 25.0, swift::misc::physical_quantities::CTimeUnit::s() },
                { 120.0, swift::misc::physical_quantities::CTimeUnit::s() }
            };
            return reader;
        }
    };

    //! Reader settings
    struct TVatsimMetars : public swift::misc::TSettingTrait<CReaderSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "vatsimreaders/metars"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("VATSIM METARs");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const swift::core::vatsim::CReaderSettings &defaultValue()
        {
            static const swift::core::vatsim::CReaderSettings reader {
                { 35.0, swift::misc::physical_quantities::CTimeUnit::s() },
                { 300.0, swift::misc::physical_quantities::CTimeUnit::s() }
            };
            return reader;
        }
    };

    //! FSD Message settings
    class SWIFT_CORE_EXPORT CRawFsdMessageSettings :
        public swift::misc::CValueObject<swift::core::vatsim::CRawFsdMessageSettings>
    {
    public:
        //! File writing mode
        enum FileWriteMode
        {
            None,
            Truncate,
            Append,
            Timestamped
        };

        //! Properties by index
        enum ColumnIndex
        {
            IndexRawFsdMessagesEnabled = swift::misc::CPropertyIndexRef::GlobalIndexRawFsdMessageSettings,
            IndexFileDir,
            IndexFileWriteMode
        };

        //! Default constructor.
        CRawFsdMessageSettings();

        //! Simplified constructor
        CRawFsdMessageSettings(bool enabled, const QString &fileDir);

        //! Are raw FSD messages enabled?
        bool areRawFsdMessagesEnabled() const { return m_rawFsdMessagesEnabled; }

        //! Get file directory
        QString getFileDir() const { return m_FileDir; }

        //! Get file write mode
        FileWriteMode getFileWriteMode() const { return m_fileWriteMode; }

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        bool m_rawFsdMessagesEnabled = false;
        QString m_FileDir;
        FileWriteMode m_fileWriteMode = None;

        SWIFT_METACLASS(
            CRawFsdMessageSettings,
            SWIFT_METAMEMBER(rawFsdMessagesEnabled),
            SWIFT_METAMEMBER(FileDir),
            SWIFT_METAMEMBER(fileWriteMode));
    };

    //! Raw FSD message settings
    struct TRawFsdMessageSetting : public swift::misc::TSettingTrait<CRawFsdMessageSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "network/rawfsdmessagelog"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("FSD message Logging");
            return name;
        }

        /* //! \copydoc swift::misc::TSettingTrait::isValid
        static bool isValid(const CRawFsdMessageSettings &setting, QString &)
        {
            if (setting.areRawFsdMessagesEnabled()) { return !setting.getFileDir().isEmpty(); }
            return true;
        }*/

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const CRawFsdMessageSettings &defaultValue()
        {
            static const CRawFsdMessageSettings setting { false, swift::misc::CSwiftDirectories::logDirectory() };
            return setting;
        }
    };
} // namespace swift::core::vatsim

Q_DECLARE_METATYPE(swift::core::vatsim::CReaderSettings)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::core::vatsim::CReaderSettings>)
Q_DECLARE_METATYPE(swift::core::vatsim::CRawFsdMessageSettings)
Q_DECLARE_METATYPE(swift::core::vatsim::CRawFsdMessageSettings::FileWriteMode)

#endif // SWIFT_CORE_VATSIM_SETTINGS_H
