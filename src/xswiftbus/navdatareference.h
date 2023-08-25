// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIM_XSWIFTBUS_NAVDATAREFERENCE_H
#define BLACKSIM_XSWIFTBUS_NAVDATAREFERENCE_H

namespace XSwiftBus
{
    //! Simplified version of CNavDataReference of \sa BlackMisc::Simulation::XPlane::CNavDataReference
    class CNavDataReference
    {
    public:
        //! Default constructor
        CNavDataReference() = default;

        //! Constructor
        CNavDataReference(int id, double latitudeDegrees, double longitudeDegrees)
            : m_id(id), m_latitudeDegrees(latitudeDegrees), m_longitudeDegrees(longitudeDegrees)
        {}

        //! \copydoc BlackMisc::Simulation::XPlane::CNavDataReference::id
        int id() const { return m_id; }

        //! \copydoc BlackMisc::Simulation::XPlane::CNavDataReference::latitude
        double latitude() const { return m_latitudeDegrees; }

        //! \copydoc BlackMisc::Simulation::XPlane::CNavDataReference::longitude
        double longitude() const { return m_longitudeDegrees; }

    private:
        int m_id = 0;
        double m_latitudeDegrees = 0.0;
        double m_longitudeDegrees = 0.0;
    };

    //! Free function to calculate great circle distance
    double calculateGreatCircleDistance(const CNavDataReference &a, const CNavDataReference &b);

} // ns

#endif // guard
