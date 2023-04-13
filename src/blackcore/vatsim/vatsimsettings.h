/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VATSIM_SETTINGS_H
#define BLACKCORE_VATSIM_SETTINGS_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/fileutils.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackCore::Vatsim, CReaderSettings)
BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackCore::Vatsim, CRawFsdMessageSettings)

namespace BlackCore::Vatsim
{
    /*!
     * Settings used with readers
     */
    class BLACKCORE_EXPORT CReaderSettings : public BlackMisc::CValueObject<BlackCore::Vatsim::CReaderSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexInitialTime = BlackMisc::CPropertyIndexRef::GlobalIndexCSettingsReaders,
            IndexPeriodicTime,
            IndexNeverUpdate
        };

        //! Default constructor.
        CReaderSettings();

        //! Simplified constructor
        CReaderSettings(const BlackMisc::PhysicalQuantities::CTime &initialTime, const BlackMisc::PhysicalQuantities::CTime &periodicTime, bool neverUpdate = false);

        //! Get time
        const BlackMisc::PhysicalQuantities::CTime &getInitialTime() const { return m_initialTime; }

        //! Set time
        void setInitialTime(const BlackMisc::PhysicalQuantities::CTime &time) { m_initialTime = time; }

        //! Get time
        const BlackMisc::PhysicalQuantities::CTime &getPeriodicTime() const { return m_periodicTime; }

        //! Set time
        void setPeriodicTime(const BlackMisc::PhysicalQuantities::CTime &time) { m_periodicTime = time; }

        //! Never ever update?
        bool isNeverUpdate() const { return m_neverUpdate; }

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Settings used when a reader is manually triggered and never updates
        static const CReaderSettings &neverUpdateSettings();

    private:
        BlackMisc::PhysicalQuantities::CTime m_initialTime { 30.0, BlackMisc::PhysicalQuantities::CTimeUnit::s() };
        BlackMisc::PhysicalQuantities::CTime m_periodicTime { 30.0, BlackMisc::PhysicalQuantities::CTimeUnit::s() };
        bool m_neverUpdate = false;

        BLACK_METACLASS(
            CReaderSettings,
            BLACK_METAMEMBER(initialTime),
            BLACK_METAMEMBER(periodicTime),
            BLACK_METAMEMBER(neverUpdate)
        );
    };

    //! Reader settings
    struct TVatsimBookings : public BlackMisc::TSettingTrait<CReaderSettings>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "vatsimreaders/bookings"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("VATSIM bookings");
            return name;
        }

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
        static const BlackCore::Vatsim::CReaderSettings &defaultValue()
        {
            static const BlackCore::Vatsim::CReaderSettings reader { { 30.0, BlackMisc::PhysicalQuantities::CTimeUnit::s() }, { 600.0, BlackMisc::PhysicalQuantities::CTimeUnit::s() } };
            return reader;
        }
    };

    //! Reader settings
    struct TVatsimDataFile : public BlackMisc::TSettingTrait<CReaderSettings>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "vatsimreaders/datafile"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("VATSIM data file");
            return name;
        }

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
        static const BlackCore::Vatsim::CReaderSettings &defaultValue()
        {
            static const BlackCore::Vatsim::CReaderSettings reader { { 25.0, BlackMisc::PhysicalQuantities::CTimeUnit::s() }, { 120.0, BlackMisc::PhysicalQuantities::CTimeUnit::s() } };
            return reader;
        }
    };

    //! Reader settings
    struct TVatsimMetars : public BlackMisc::TSettingTrait<CReaderSettings>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "vatsimreaders/metars"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("VATSIM METARs");
            return name;
        }

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
        static const BlackCore::Vatsim::CReaderSettings &defaultValue()
        {
            static const BlackCore::Vatsim::CReaderSettings reader { { 35.0, BlackMisc::PhysicalQuantities::CTimeUnit::s() }, { 300.0, BlackMisc::PhysicalQuantities::CTimeUnit::s() } };
            return reader;
        }
    };

    //! FSD Message settings
    class BLACKCORE_EXPORT CRawFsdMessageSettings : public BlackMisc::CValueObject<BlackCore::Vatsim::CRawFsdMessageSettings>
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
            IndexRawFsdMessagesEnabled = BlackMisc::CPropertyIndexRef::GlobalIndexRawFsdMessageSettings,
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

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::String::toQString
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
    struct TRawFsdMessageSetting : public BlackMisc::TSettingTrait<CRawFsdMessageSettings>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "network/rawfsdmessagelog"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("FSD message Logging");
            return name;
        }

        /* //! \copydoc BlackMisc::TSettingTrait::isValid
        static bool isValid(const CRawFsdMessageSettings &setting, QString &)
        {
            if (setting.areRawFsdMessagesEnabled()) { return !setting.getFileDir().isEmpty(); }
            return true;
        }*/

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
        static const CRawFsdMessageSettings &defaultValue()
        {
            static const CRawFsdMessageSettings setting { false, BlackMisc::CSwiftDirectories::logDirectory() };
            return setting;
        }
    };
} // ns

Q_DECLARE_METATYPE(BlackCore::Vatsim::CReaderSettings)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackCore::Vatsim::CReaderSettings>)
Q_DECLARE_METATYPE(BlackCore::Vatsim::CRawFsdMessageSettings)
Q_DECLARE_METATYPE(BlackCore::Vatsim::CRawFsdMessageSettings::FileWriteMode)

#endif
