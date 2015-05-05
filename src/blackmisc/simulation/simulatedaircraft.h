/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATEDAIRCRAFT_H
#define BLACKMISC_SIMULATION_SIMULATEDAIRCRAFT_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/aircraft.h"
#include "aircraftmodel.h"
#include "blackmisc/network/client.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Comprehensive information of an aircraft
        //! \sa CAircraft
        class BLACKMISC_EXPORT CSimulatedAircraft : public CValueObject<CSimulatedAircraft, BlackMisc::Aviation::CAircraft>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexModel = BlackMisc::CPropertyIndex::GlobalIndexCSimulatedAircraft,
                IndexClient,
                IndexEnabled,
                IndexRendered,
                IndexPartsSynchronized,
                IndexFastPositionUpdates
            };

            //! Default constructor.
            CSimulatedAircraft();

            //! Constructor.
            CSimulatedAircraft(const BlackMisc::Aviation::CAircraft &aircraft,
                               const BlackMisc::Simulation::CAircraftModel &model = {},
                               const BlackMisc::Network::CClient &client = {});

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Get model
            const BlackMisc::Simulation::CAircraftModel &getModel() const { return m_model; }

            //! Get model string
            QString getModelString() const { return m_model.getModelString(); }

            //! Set model
            void setModel(const BlackMisc::Simulation::CAircraftModel &model);

            //! Set model string
            void setModelString(const QString &modelString);

            //! \copydoc CAircraft::setCallsign
            virtual void setCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc CAircraft::setIcaoInfo
            virtual void setIcaoInfo(const BlackMisc::Aviation::CAircraftIcao &icao) override;

            //! \copydoc CAircraft::setPilot
            virtual void setPilot(const BlackMisc::Network::CUser &user) override;

            //! Get client
            const BlackMisc::Network::CClient &getClient() const { return m_client; }

            //! Set client
            void setClient(const BlackMisc::Network::CClient &client);

            //! Enabled?
            bool isEnabled() const { return m_enabled; }

            //! Enabled / disabled
            void setEnabled(bool enabled) { m_enabled = enabled; }

            //! Rendered?
            bool isRendered() const { return m_rendered; }

            //! Support fast position updates
            bool fastPositionUpdates() const {return m_fastPositionUpdates;}

            //! Support fast position updates
            void setFastPositionUpdates(bool useFastPositions) { m_fastPositionUpdates = useFastPositions; }

            //! Rendered?
            void setRendered(bool rendered) { m_rendered = rendered; }

            //! Update from aviation aircraft
            void setAircraft(const BlackMisc::Aviation::CAircraft &aircraft);

            //! Have parts been synchronized with a remote client?
            bool isPartsSynchronized() const { return m_partsSynchronized; }

            //! Set the synchronisation flag
            void setPartsSynchronized(bool synchronized) { m_partsSynchronized = synchronized; }

            //! \copydoc CValueObject::convertToQString()
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CSimulatedAircraft)
            BlackMisc::Simulation::CAircraftModel m_model;
            BlackMisc::Network::CClient m_client;
            bool m_enabled = true;              //!< to be displayed in sim
            bool m_rendered = false;            //!< really shown in simulator
            bool m_partsSynchronized = false;   //!< sync.parts
            bool m_fastPositionUpdates =false;  //!<use fast position updates

            void init();

        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Simulation::CSimulatedAircraft, (
                                   attr(o.m_model),
                                   attr(o.m_client),
                                   attr(o.m_enabled),
                                   attr(o.m_rendered),
                                   attr(o.m_partsSynchronized),
                                   attr(o.m_fastPositionUpdates)
                               ))
Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatedAircraft)

#endif // guard
