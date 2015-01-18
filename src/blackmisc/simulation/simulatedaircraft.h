/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATEDAIRCRAFT_H
#define BLACKMISC_SIMULATEDAIRCRAFT_H

#include "blackmisc/avaircraft.h"
#include "aircraftmodel.h"
#include "blackmisc/nwclient.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Comprehensive information of an aircraft
        //! \sa CAircraft
        class CSimulatedAircraft : public CValueObjectStdTuple<CSimulatedAircraft, BlackMisc::Aviation::CAircraft>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexModel = BlackMisc::CPropertyIndex::GlobalIndexCSimulatedAircraft,
                IndexClient,
                IndexEnabled
            };

            //! Default constructor.
            CSimulatedAircraft();

            //! Constructor.
            CSimulatedAircraft(const BlackMisc::Aviation::CAircraft &aircraft,
                               const BlackMisc::Simulation::CAircraftModel &model = {},
                               const BlackMisc::Network::CClient &client = {});

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! Get model
            const BlackMisc::Simulation::CAircraftModel &getModel() const { return m_model; }

            //! Set model
            void setModel(const BlackMisc::Simulation::CAircraftModel &model);

            //! \copydoc CAircraft::setCallsign
            virtual void setCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc CAircraft::setIcaoInfo
            virtual void setIcaoInfo(const BlackMisc::Aviation::CAircraftIcao &icao) override;

            //! \copydoc CAircraft::setPilot
            virtual void setPilot(const BlackMisc::Network::CUser &user);

            //! Get client
            const BlackMisc::Network::CClient &getClient() const { return m_client; }

            //! Set client
            void setClient(const BlackMisc::Network::CClient &client);

            //! Enabled?
            bool isEnabled() const { return m_enabled; }

            //! Enabled
            void setEnabled(bool enabled) { m_enabled = enabled; }

            //! Update from aviation aircraft
            void setAircraft(const BlackMisc::Aviation::CAircraft &aircraft);

        protected:
            //! \copydoc CValueObject::convertToQString()
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CSimulatedAircraft)
            BlackMisc::Simulation::CAircraftModel m_model;
            BlackMisc::Network::CClient        m_client;
            bool                               m_enabled = true;
            void init();

        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Simulation::CSimulatedAircraft, (
                                   attr(o.m_model),
                                   attr(o.m_client),
                                   attr(o.m_enabled)
                               ))
Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatedAircraft)

#endif // guard
