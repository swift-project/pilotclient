//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATION_INTERPOLATIONSTATUS_H
#define BLACKMISC_SIMULATION_INTERPOLATION_INTERPOLATIONSTATUS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/aircraftsituation.h"

#include <QString>

namespace BlackMisc::Simulation
{
    //! Status of interpolation
    class BLACKMISC_EXPORT CInterpolationStatus
    {
    public:
        //! Did interpolation succeed?
        bool isInterpolated() const { return m_isInterpolated; }

        //! Set succeeded
        void setInterpolated(bool interpolated) { m_isInterpolated = interpolated; }

        //! Interpolating between 2 same situations?
        bool isSameSituation() const { return m_isSameSituation; }

        //! Interpolating between 2 same situations?
        void setSameSituation(bool same) { m_isSameSituation = same; }

        //! Set situations count
        void setSituationsCount(int count) { m_situations = count; }

        //! Extra info
        void setExtraInfo(const QString &info);

        //! Set succeeded
        void setInterpolatedAndCheckSituation(bool succeeded, const Aviation::CAircraftSituation &situation);

        //! Is the corresponding position valid?
        bool hasValidSituation() const { return m_isValidSituation; }

        //! Valid interpolated situation
        bool hasValidInterpolatedSituation() const;

        //! Is that a valid position?
        void checkIfValidSituation(const Aviation::CAircraftSituation &situation);

        //! Info string
        QString toQString() const;

    private:
        bool m_isInterpolated = false; //!< position is interpolated (means enough values, etc.)
        bool m_isValidSituation = false; //!< is valid situation
        bool m_isSameSituation = false; //!< interpolation between 2 same situations
        int m_situations = -1; //!< number of situations used for interpolation
        QString m_extraInfo; //!< optional details
    };
}

#endif // BLACKMISC_SIMULATION_INTERPOLATION_INTERPOLATIONSTATUS_H
