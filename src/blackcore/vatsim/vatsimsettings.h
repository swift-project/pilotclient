// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_VATSIM_SETTINGS_H
#define BLACKCORE_VATSIM_SETTINGS_H

#include "blackcore/blackcoreexport.h"
#include "misc/settingscache.h"
#include "misc/valueobject.h"
#include "misc/pq/time.h"
#include "misc/network/serverlist.h"
#include "misc/swiftdirectories.h"
#include "misc/directoryutils.h"
#include "misc/fileutils.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackCore::Vatsim, CReaderSettings)
BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackCore::Vatsim, CRawFsdMessageSettings)

namespace BlackCore::Vatsim
{
    /*!
     * Settings used with readers
     */
    class BLACKCORE_EXPORT CReaderSettings : public swift::misc::CValueObject<BlackCore::Vatsim::CReaderSettings>
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
        CReaderSettings(const swift::misc::physical_quantities::CTime &initialTime, const swift::misc::physical_quantities::CTime &periodicTime, bool neverUpdate = false);

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
        swift::misc::physical_quantities::CTime m_initialTime { 30.0, swift::misc::physical_quantities::CTimeUnit::s() };
        swift::misc::physical_quantities::CTime m_periodicTime { 30.0, swift::misc::physical_quantities::CTimeUnit::s() };
        bool m_neverUpdate = false;

        BLACK_METACLASS(
            CReaderSettings,
            BLACK_METAMEMBER(initialTime),
            BLACK_METAMEMBER(periodicTime),
            BLACK_METAMEMBER(neverUpdate)
        );
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
        static const BlackCore::Vatsim::CReaderSettings &defaultValue()
        {
            static const BlackCore::Vatsim::CReaderSettings reader { { 25.0, swift::misc::physical_quantities::CTimeUnit::s() }, { 120.0, swift::misc::physical_quantities::CTimeUnit::s() } };
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
        static const BlackCore::Vatsim::CReaderSettings &defaultValue()
        {
            static const BlackCore::Vatsim::CReaderSettings reader { { 35.0, swift::misc::physical_quantities::CTimeUnit::s() }, { 300.0, swift::misc::physical_quantities::CTimeUnit::s() } };
            return reader;
        }
    };

    //! FSD Message settings
    class BLACKCORE_EXPORT CRawFsdMessageSettings : public swift::misc::CValueObject<BlackCore::Vatsim::CRawFsdMessageSettings>
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

        BLACK_METACLASS(
            CRawFsdMessageSettings,
            BLACK_METAMEMBER(rawFsdMessagesEnabled),
            BLACK_METAMEMBER(FileDir),
            BLACK_METAMEMBER(fileWriteMode)
        );
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
} // ns

Q_DECLARE_METATYPE(BlackCore::Vatsim::CReaderSettings)
Q_DECLARE_METATYPE(swift::misc::CCollection<BlackCore::Vatsim::CReaderSettings>)
Q_DECLARE_METATYPE(BlackCore::Vatsim::CRawFsdMessageSettings)
Q_DECLARE_METATYPE(BlackCore::Vatsim::CRawFsdMessageSettings::FileWriteMode)

#endif
