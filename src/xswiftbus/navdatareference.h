/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
