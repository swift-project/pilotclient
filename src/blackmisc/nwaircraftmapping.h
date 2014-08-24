/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

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
            enum ColumnIndex
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
            QVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index) override;

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

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::convertFromJson
            virtual void convertFromJson(const QJsonObject &json) override;

            //! Matches model string?
            bool matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const;

            //! Matches wildcard icao object
            bool matchesWildcardIcao(const BlackMisc::Aviation::CAircraftIcao &otherIcao) const { return m_icao.matchesWildcardIcao(otherIcao); }

            //! Register the metatypes
            static void registerMetadata();

            //! Members
            static const QStringList &jsonMembers();
        };
    }
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CAircraftMapping, (o.m_icao, o.m_model))
Q_DECLARE_METATYPE(BlackMisc::Network::CAircraftMapping)

#endif // guard
