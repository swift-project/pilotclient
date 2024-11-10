// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_AIRSPACEAIRCRAFTANALYZER_H
#define SWIFT_MISC_SIMULATION_AIRSPACEAIRCRAFTANALYZER_H

#include "misc/aviation/callsignset.h"
#include "misc/swiftmiscexport.h"
#include "misc/metaclass.h"
#include "misc/pq/length.h"
#include "misc/pq/units.h"
#include "misc/propertyindexref.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/valueobject.h"

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <QtGlobal>

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation, CAirspaceAircraftSnapshot)

namespace swift::misc::simulation
{
    //! Current situation in the skies analyzed.
    class SWIFT_MISC_EXPORT CAirspaceAircraftSnapshot : public CValueObject<CAirspaceAircraftSnapshot>
    {
    public:
        //! Default constructor
        CAirspaceAircraftSnapshot();

        //! Constructor
        CAirspaceAircraftSnapshot(const CSimulatedAircraftList &allAircraft,
                                  bool restricted = false,
                                  bool renderingEnabled = true,
                                  int maxAircraft = 100,
                                  const swift::misc::physical_quantities::CLength &maxRenderedDistance = { 0, nullptr });

        //! Time when snapshot was taken
        const QDateTime getTimestamp() const { return QDateTime::fromMSecsSinceEpoch(m_timestampMsSinceEpoch); }

        //! Callsigns by distance
        const swift::misc::aviation::CCallsignSet &getAircraftCallsignsByDistance() const { return m_aircraftCallsignsByDistance; }

        //! Callsigns by distance, only enabled aircraft
        const swift::misc::aviation::CCallsignSet &getEnabledAircraftCallsignsByDistance() const { return m_enabledAircraftCallsignsByDistance; }

        //! Callsigns by distance, only disabled aircraft
        const swift::misc::aviation::CCallsignSet &getDisabledAircraftCallsignsByDistance() const { return m_disabledAircraftCallsignsByDistance; }

        //! VTOL aircraft callsigns by distance, only enabled aircraft
        const swift::misc::aviation::CCallsignSet &getVtolAircraftCallsignsByDistance() const { return m_vtolAircraftCallsignsByDistance; }

        //! VTOL aircraft callsigns by distance, only enabled aircraft
        const swift::misc::aviation::CCallsignSet &getEnabledVtolAircraftCallsignsByDistance() const { return m_enabledVtolAircraftCallsignsByDistance; }

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

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
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
        swift::misc::aviation::CCallsignSet m_aircraftCallsignsByDistance;

        swift::misc::aviation::CCallsignSet m_enabledAircraftCallsignsByDistance;
        swift::misc::aviation::CCallsignSet m_disabledAircraftCallsignsByDistance;

        swift::misc::aviation::CCallsignSet m_vtolAircraftCallsignsByDistance;
        swift::misc::aviation::CCallsignSet m_enabledVtolAircraftCallsignsByDistance;

        SWIFT_METACLASS(
            CAirspaceAircraftSnapshot,
            SWIFT_METAMEMBER(timestampMsSinceEpoch),
            SWIFT_METAMEMBER(aircraftCallsignsByDistance, 0, DisabledForComparison),
            SWIFT_METAMEMBER(enabledAircraftCallsignsByDistance, 0, DisabledForComparison),
            SWIFT_METAMEMBER(disabledAircraftCallsignsByDistance, 0, DisabledForComparison),
            SWIFT_METAMEMBER(vtolAircraftCallsignsByDistance, 0, DisabledForComparison),
            SWIFT_METAMEMBER(enabledVtolAircraftCallsignsByDistance, 0, DisabledForComparison));
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::simulation::CAirspaceAircraftSnapshot)

#endif
