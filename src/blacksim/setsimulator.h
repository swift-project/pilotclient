/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file

#ifndef BLACKMISC_SETTINGS_SIMULATOR_H
#define BLACKMISC_SETTINGS_SIMULATOR_H

#include "blackmisc/valueobject.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/settingutilities.h"
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

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Selected driver
            BlackSim::CSimulatorInfo getSelectedDriver() const { return this->m_selectedDriver; }

            //! Selected driver
            bool setSelectedDriver(const BlackSim::CSimulatorInfo &driver) { this->m_selectedDriver = driver; }

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
            BlackSim::CSimulatorInfo m_selectedDriver;
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackSim::Settings::CSettingsSimulator)
BLACK_DECLARE_TUPLE_CONVERSION(BlackSim::Settings::CSettingsSimulator, (o.m_selectedDriver))

#endif // guard
