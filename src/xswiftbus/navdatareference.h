// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIM_XSWIFTBUS_NAVDATAREFERENCE_H
#define SWIFT_SIM_XSWIFTBUS_NAVDATAREFERENCE_H

namespace XSwiftBus
{
    //! Simplified version of CNavDataReference of \sa swift::misc::simulation::XPlane::CNavDataReference
    class CNavDataReference
    {
    public:
        //! Default constructor
        CNavDataReference() = default;

        //! Constructor
        CNavDataReference(int id, double latitudeDegrees, double longitudeDegrees)
            : m_id(id), m_latitudeDegrees(latitudeDegrees), m_longitudeDegrees(longitudeDegrees)
        {}

        //! \copydoc swift::misc::simulation::XPlane::CNavDataReference::id
        int id() const { return m_id; }

        //! \copydoc swift::misc::simulation::XPlane::CNavDataReference::latitude
        double latitude() const { return m_latitudeDegrees; }

        //! \copydoc swift::misc::simulation::XPlane::CNavDataReference::longitude
        double longitude() const { return m_longitudeDegrees; }

    private:
        int m_id = 0;
        double m_latitudeDegrees = 0.0;
        double m_longitudeDegrees = 0.0;
    };

    //! Free function to calculate great circle distance
    double calculateGreatCircleDistance(const CNavDataReference &a, const CNavDataReference &b);

} // namespace XSwiftBus

#endif // guard
