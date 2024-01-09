// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLANT_H
#define BLACKMISC_SIMULATION_INTERPOLANT_H

#include "blackmisc/simulation/interpolatorlinearpbh.h"

namespace BlackMisc::Simulation
{
    //! Interpolant interface
    class IInterpolant
    {
    public:
        //! "Real time" representing the interpolated situation
        qint64 getInterpolatedTime() const { return m_interpolatedTime; }

        //! Situations available
        int getSituationsAvailable() const { return m_situationsAvailable; }

        //! Valid?
        bool isValid() const { return m_valid; }

        //! Valid?
        void setValid(bool valid) { m_valid = valid; }

        //! Is recalculated interpolant?
        bool isRecalculated() const { return m_recalculated; }

        //! Set recalculated interpolant
        void setRecalculated(bool reCalculated) { m_recalculated = reCalculated; }

    protected:
        //! Default ctor
        IInterpolant() = default;

        //! Constructor
        explicit IInterpolant(int situationsAvailable) : m_situationsAvailable(situationsAvailable) {}

        //! Constructor
        IInterpolant(qint64 interpolatedTime, int situationsAvailable) : m_interpolatedTime(interpolatedTime), m_situationsAvailable(situationsAvailable) {}

        qint64 m_interpolatedTime = -1; //!< "Real time "of interpolated situation
        int m_situationsAvailable = 0; //!< used situations
        bool m_valid = true; //!< valid?
        bool m_recalculated = false; //!< recalculated interpolant
    };
} // namespace
#endif // guard
