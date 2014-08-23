/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SETTINGS_SIMULATOR_H
#define BLACKMISC_SETTINGS_SIMULATOR_H

#include "blackmisc/valueobject.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/settingutilities.h"
#include "blackmisc/pqtime.h"
#include "simulatorinfo.h"

namespace BlackSim
{
    namespace Settings
    {
        //! Value object encapsulating information of simulator related settings.
        class CSettingsSimulator : public BlackMisc::CValueObject
        {
        public:
            //! Default constructor.
            CSettingsSimulator();

            //! Destructor.
            virtual ~CSettingsSimulator() {}

            //! Path
            static const QString &ValueSelectedDriver()
            {
                static const QString value("selecteddriver");
                return value;
            }

            //! Path
            static const QString &ValueSyncTimeOffset()
            {
                static const QString value("synctimeoffset");
                return value;
            }

            //! Path
            static const QString &ValueSyncTime()
            {
                static const QString value("synctime");
                return value;
            }

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Selected driver
            const BlackSim::CSimulatorInfo &getSelectedPlugin() const { return this->m_selectedPlugin; }

            //! Selected driver
            void setSelectedPlugin(const BlackSim::CSimulatorInfo &plugin) { this->m_selectedPlugin = plugin; }

            //! Time synchronization offset time
            const BlackMisc::PhysicalQuantities::CTime &getSyncTimeOffset() const { return this->m_timeSyncOffset;}

            //! Set time synchronization offset time
            void setSyncTimeOffset(const BlackMisc::PhysicalQuantities::CTime &offset) { this->m_timeSyncOffset = offset; this->m_timeSyncOffset.switchUnit(BlackMisc::PhysicalQuantities::CTimeUnit::hrmin());}

            //! Time syncronization enabled?
            bool isTimeSyncEnabled() const { return this->m_timeSync;}

            //! Set time synchronization
            void setTimeSyncEnabled(bool enabled) { this->m_timeSync = enabled; }

            //! Equal operator ==
            bool operator ==(const CSettingsSimulator &other) const;

            //! Unequal operator !=
            bool operator !=(const CSettingsSimulator &other) const;

            //! \copydoc BlackCore::IContextSettings
            virtual BlackMisc::CStatusMessageList value(const QString &path, const QString &command, const BlackMisc::CVariant &value, bool &changedFlag);

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            virtual void fromJson(const QJsonObject &json) override;

            //! Init with meaningful default values
            void initDefaultValues();

            //! \copydoc CValueObject::registerMetadata
            static void registerMetadata();

            //! \copydoc TupleConverter<>::jsonMembers()
            static const QStringList &jsonMembers();

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CSettingsSimulator)
            BlackSim::CSimulatorInfo m_selectedPlugin;
            bool m_timeSync = false;
            BlackMisc::PhysicalQuantities::CTime m_timeSyncOffset;
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackSim::Settings::CSettingsSimulator)
BLACK_DECLARE_TUPLE_CONVERSION(BlackSim::Settings::CSettingsSimulator, (o.m_selectedPlugin, o.m_timeSync, o.m_timeSyncOffset))

#endif // guard
