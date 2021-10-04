/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AIRSPACEAIRCRAFTANALYZER_H
#define BLACKMISC_SIMULATION_AIRSPACEAIRCRAFTANALYZER_H

#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/valueobject.h"

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <QtGlobal>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Simulation, CAirspaceAircraftSnapshot)

namespace BlackMisc::Simulation
{
    //! Current situation in the skies analyzed.
    class BLACKMISC_EXPORT CAirspaceAircraftSnapshot : public CValueObject<CAirspaceAircraftSnapshot>
    {
    public:
        //! Default constructor
        CAirspaceAircraftSnapshot();

        //! Constructor
        CAirspaceAircraftSnapshot(const CSimulatedAircraftList &allAircraft,
                                    bool restricted       = false,
                                    bool renderingEnabled = true,
                                    int maxAircraft       = 100,
                                    const BlackMisc::PhysicalQuantities::CLength &maxRenderedDistance = { 0, nullptr });

        //! Time when snapshot was taken
        const QDateTime getTimestamp() const { return QDateTime::fromMSecsSinceEpoch(m_timestampMsSinceEpoch); }

        //! Callsigns by distance
        const BlackMisc::Aviation::CCallsignSet &getAircraftCallsignsByDistance() const { return m_aircraftCallsignsByDistance; }

        //! Callsigns by distance, only enabled aircraft
        const BlackMisc::Aviation::CCallsignSet &getEnabledAircraftCallsignsByDistance() const { return m_enabledAircraftCallsignsByDistance; }

        //! Callsigns by distance, only disabled aircraft
        const BlackMisc::Aviation::CCallsignSet &getDisabledAircraftCallsignsByDistance() const { return m_disabledAircraftCallsignsByDistance; }

        //! VTOL aircraft callsigns by distance, only enabled aircraft
        const BlackMisc::Aviation::CCallsignSet &getVtolAircraftCallsignsByDistance() const { return m_vtolAircraftCallsignsByDistance; }

        //! VTOL aircraft callsigns by distance, only enabled aircraft
        const BlackMisc::Aviation::CCallsignSet &getEnabledVtolAircraftCallsignsByDistance() const { return m_enabledVtolAircraftCallsignsByDistance; }

        //! Valid snapshot?
        bool isValidSnapshot() const;

        //! Restricted snapshot?
        bool isValidRestricted() const { return m_restricted; }

        //! Did restriction change compared to last snapshot
        void setRestrictionChanged(const CAirspaceAircraftSnapshot &snapshot);

        //! Did the restriction flag change?
        bool isRestrictionChanged() const { return m_restrictionChanged; }

        //! Restricted values?
        bool isRestricted() const { return m_restricted; }

        //! Rendering enabled or all aircraft disabled?
        bool isRenderingEnabled() const { return m_renderingEnabled; }

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Generating thread name
        const QString &generatingThreadName() const { return m_threadName; }

    private:
        qint64 m_timestampMsSinceEpoch = -1;
        bool m_restricted = false;
        bool m_renderingEnabled = true;
        bool m_restrictionChanged = false;
        QString m_threadName; //!< generating thread name for debugging purposes

        // remark closest aircraft always first
        BlackMisc::Aviation::CCallsignSet m_aircraftCallsignsByDistance;

        BlackMisc::Aviation::CCallsignSet m_enabledAircraftCallsignsByDistance;
        BlackMisc::Aviation::CCallsignSet m_disabledAircraftCallsignsByDistance;

        BlackMisc::Aviation::CCallsignSet m_vtolAircraftCallsignsByDistance;
        BlackMisc::Aviation::CCallsignSet m_enabledVtolAircraftCallsignsByDistance;

        BLACK_METACLASS(
            CAirspaceAircraftSnapshot,
            BLACK_METAMEMBER(timestampMsSinceEpoch),
            BLACK_METAMEMBER(aircraftCallsignsByDistance, 0, DisabledForComparison),
            BLACK_METAMEMBER(enabledAircraftCallsignsByDistance, 0, DisabledForComparison),
            BLACK_METAMEMBER(disabledAircraftCallsignsByDistance, 0, DisabledForComparison),
            BLACK_METAMEMBER(vtolAircraftCallsignsByDistance, 0, DisabledForComparison),
            BLACK_METAMEMBER(enabledVtolAircraftCallsignsByDistance, 0, DisabledForComparison)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CAirspaceAircraftSnapshot)

#endif
