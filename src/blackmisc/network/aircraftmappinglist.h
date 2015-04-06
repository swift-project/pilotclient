/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_AIRCRAFTMAPPINGLIST_H
#define BLACKMISC_NETWORK_AIRCRAFTMAPPINGLIST_H

#include "blackmisc/network/aircraftmapping.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Network
    {
        //! Value object encapsulating a list of aircraft mappings
        class CAircraftMappingList : public CSequence<CAircraftMapping>
        {
        public:
            //! Empty constructor.
            CAircraftMappingList();

            //! Construct from a base class object.
            CAircraftMappingList(const CSequence<CAircraftMapping> &other);

            //! Find by ICAO code, empty fields treated as wildcards
            CAircraftMappingList findByIcaoCodeWildcard(const BlackMisc::Aviation::CAircraftIcao &searchIcao) const;

            //! Find by ICAO aircraft designator
            CAircraftMappingList findByIcaoAircraftDesignator(const BlackMisc::Aviation::CAircraftIcao &searchIcao) const;

            //! Find by ICAO airline designator
            CAircraftMappingList findByIcaoAirlineDesignator(const BlackMisc::Aviation::CAircraftIcao &searchIcao) const;

            //! Find by ICAO aircraft and airline designator
            CAircraftMappingList findByIcaoAircraftAndAirlineDesignator(const BlackMisc::Aviation::CAircraftIcao &searchIcao, bool allowRelaxedAirline) const;

            //! Find by ICAO code, empty fields treated literally
            CAircraftMappingList findByIcaoCodeExact(const BlackMisc::Aviation::CAircraftIcao &searchIcao) const;

            //! Find by model string
            CAircraftMappingList findByModelString(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Register metadata
            static void registerMetadata();
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CAircraftMappingList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CAircraftMapping>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Network::CAircraftMapping>)

#endif //guard
