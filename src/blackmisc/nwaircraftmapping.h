/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_NETWORK_AIRCRAFTMAPPING_H
#define BLACKMISC_NETWORK_AIRCRAFTMAPPING_H

#include "blackmisc/avaircrafticao.h"
#include "blackmisc/nwaircraftmodel.h"
#include "blackmisc/valueobject.h"

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Mapping
         */
        class CAircraftMapping: public BlackMisc::CValueObject
        {
        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftMapping)
            BlackMisc::Aviation::CAircraftIcao m_icao; //!< ICAO code
            BlackMisc::Network::CAircraftModel m_model; //!< aircraft model

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &) override;

            //! \copydoc CValueObject::compareImpl(const CValueObject &)
            int compareImpl(const CValueObject &otherBase) const override;

            //! \copydoc CValueObject::getMetaTypeId()
            int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

        public:
            //! Properties
            enum IndexProperties
            {
                IndexModel,
                IndexIcaoCode
            };

            //! Default constructor
            CAircraftMapping() {}

            //! Constructor
            CAircraftMapping(const QString &aircraftDesignator, const QString &airlineDesignator, const QString &model);

            //! Virtual destructor
            virtual ~CAircraftMapping() {}

            //! operator ==
            bool operator ==(const CAircraftMapping &other) const;

            //! operator !=
            bool operator !=(const CAircraftMapping &other) const;

            //! \copydoc CValueObject::propertyByIndex
            QVariant propertyByIndex(int index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const QVariant &variant, int index) override;

            //! ICAO
            void setIcao(const BlackMisc::Aviation::CAircraftIcao &icao) { this->m_icao = icao; }

            //! ICAO
            const BlackMisc::Aviation::CAircraftIcao &getIcao() const { return this->m_icao; }

            //! Model
            void setModel(const BlackMisc::Network::CAircraftModel &model) { this->m_model = model; }

            //! Model
            const BlackMisc::Network::CAircraftModel &getModel() const { return this->m_model; }

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Matches model string?
            bool matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const;

            //! Register the metatypes
            static void registerMetadata();

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            virtual void fromJson(const QJsonObject &json) override;

            //! Members
            static const QStringList &jsonMembers();
        };
    }
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CAircraftMapping, (o.m_icao, o.m_model))
Q_DECLARE_METATYPE(BlackMisc::Network::CAircraftMapping)

#endif // guard
