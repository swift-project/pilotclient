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

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/aircrafticao.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/valueobject.h"

namespace BlackMisc
{
    namespace Network
    {
        //! Mapping
        class BLACKMISC_EXPORT CAircraftMapping : public CValueObject<CAircraftMapping>
        {

        public:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! Properties
            enum ColumnIndex
            {
                IndexModel = BlackMisc::CPropertyIndex::GlobalIndexCAircraftMapping,
                IndexIcao,
                IndexPackageName,
                IndexSource
            };

            //! Default constructor
            CAircraftMapping() = default;

            //! Constructor
            CAircraftMapping(const QString &source, const QString &packageName, const QString &aircraftDesignator, const QString &airlineDesignator, const QString &model);

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! ICAO
            void setIcao(const BlackMisc::Aviation::CAircraftIcao &icao) { this->m_icao = icao; }

            //! ICAO
            const BlackMisc::Aviation::CAircraftIcao &getIcao() const { return this->m_icao; }

            //! Model
            void setModel(const BlackMisc::Simulation::CAircraftModel &model) { this->m_model = model; }

            //! Model
            const BlackMisc::Simulation::CAircraftModel &getModel() const { return this->m_model; }

            //! Matches model string?
            bool matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const;

            //! Matches wildcard icao object
            bool matchesWildcardIcao(const BlackMisc::Aviation::CAircraftIcao &otherIcao) const { return m_icao.matchesWildcardIcao(otherIcao); }

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftMapping)

            QString                            m_source;         //!< source, e.g. database, vPilot
            QString                            m_packageName;    //!< something like WoA, ..
            BlackMisc::Aviation::CAircraftIcao m_icao;           //!< ICAO code
            BlackMisc::Simulation::CAircraftModel m_model;          //!< aircraft model

            // BlackMisc::Simulation::CSimulatorPluginInfo m_simulatorInfo; //!< Mapping is for simulator
        };
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CAircraftMapping, (o.m_icao, o.m_model))
Q_DECLARE_METATYPE(BlackMisc::Network::CAircraftMapping)

#endif // guard
