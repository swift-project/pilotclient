/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_SETTINGS_READER_H
#define BLACKCORE_SETTINGS_READER_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/pq/time.h"

namespace BlackCore
{
    namespace Settings
    {
        /*!
         * Settings used with readers
         */
        class BLACKCORE_EXPORT CSettingsReader : public BlackMisc::CValueObject<BlackCore::Settings::CSettingsReader>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexInitialTime = BlackMisc::CPropertyIndex::GlobalIndexCSettingsReaders,
                IndexPeriodicTime,
                IndexNeverUpdate
            };

            //! Default constructor.
            CSettingsReader();

            //! Simplified constructor
            CSettingsReader(const BlackMisc::PhysicalQuantities::CTime &initialTime, const BlackMisc::PhysicalQuantities::CTime &periodicTime, bool neverUpdate = false);

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
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Settings used when a reader is manually triggered and never updates
            static const CSettingsReader &neverUpdateSettings();

        private:
            BlackMisc::PhysicalQuantities::CTime m_initialTime { 30.0, BlackMisc::PhysicalQuantities::CTimeUnit::s()};
            BlackMisc::PhysicalQuantities::CTime m_periodicTime { 30.0, BlackMisc::PhysicalQuantities::CTimeUnit::s()};
            bool m_neverUpdate = false;

            BLACK_METACLASS(
                CSettingsReader,
                BLACK_METAMEMBER(initialTime),
                BLACK_METAMEMBER(periodicTime),
                BLACK_METAMEMBER(neverUpdate)
            );
        };

        //! Reader settings
        struct SettingsVatsimBookings : public BlackMisc::CSettingTrait<CSettingsReader>
        {
            //! \copydoc BlackCore::CSettingTrait::key
            static const char *key() { return "vatsimreaders/bookings"; }

            //! \copydoc BlackCore::CSettingTrait::defaultValue
            static const BlackCore::Settings::CSettingsReader &defaultValue()
            {
                static const BlackCore::Settings::CSettingsReader reader {{30.0, BlackMisc::PhysicalQuantities::CTimeUnit::s()}, {120.0, BlackMisc::PhysicalQuantities::CTimeUnit::s()}};
                return reader;
            }
        };

        //! Reader settings
        struct SettingsVatsimDataFile : public BlackMisc::CSettingTrait<CSettingsReader>
        {
            //! \copydoc BlackCore::CSettingTrait::key
            static const char *key() { return "vatsimreaders/datafile"; }

            //! \copydoc BlackCore::CSettingTrait::defaultValue
            static const BlackCore::Settings::CSettingsReader &defaultValue()
            {
                static const BlackCore::Settings::CSettingsReader reader {{25.0, BlackMisc::PhysicalQuantities::CTimeUnit::s()}, {120.0, BlackMisc::PhysicalQuantities::CTimeUnit::s()}};
                return reader;
            }
        };

        //! Reader settings
        struct SettingsVatsimMetars : public BlackMisc::CSettingTrait<CSettingsReader>
        {
            //! \copydoc BlackCore::CSettingTrait::key
            static const char *key() { return "vatsimreaders/metars"; }

            //! \copydoc BlackCore::CSettingTrait::defaultValue
            static const BlackCore::Settings::CSettingsReader &defaultValue()
            {
                static const BlackCore::Settings::CSettingsReader reader {{35.0, BlackMisc::PhysicalQuantities::CTimeUnit::s()}, {300.0, BlackMisc::PhysicalQuantities::CTimeUnit::s()}};
                return reader;
            }
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackCore::Settings::CSettingsReader)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackCore::Settings::CSettingsReader>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackCore::Settings::CSettingsReader>)

#endif
