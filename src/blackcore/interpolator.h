/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_INTERPOLATOR_H
#define BLACKCORE_INTERPOLATOR_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"

#include <QObject>
#include <QString>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Aviation
    {
        class CCallsign;
    }
}

namespace BlackCore
{
    //! Interpolator, calculation inbetween positions
    class BLACKCORE_EXPORT IInterpolator :
        public QObject,
        public BlackMisc::Simulation::CRemoteAircraftAware
    {
        Q_OBJECT

    public:
        //! Virtual destructor
        virtual ~IInterpolator() {}

        //! Log category
        static QString getMessageCategory() { return "swift.interpolator"; }

        //! Status of interpolation
        struct BLACKCORE_EXPORT InterpolationStatus
        {
        public:
            bool changedPosition = false;        //!< position was changed
            bool interpolationSucceeded = false; //!< interpolation succeeded (means enough values, etc.)

            //! all OK
            bool allTrue() const;

            //! Reset to default values
            void reset();
        };

        //! Status regarding parts
        struct PartsStatus
        {
            bool supportsParts = false;   //!< supports parts for given callsign

            //! all OK
            bool allTrue() const;

            //! Reset to default values
            void reset();
        };

        //! Current interpolated situation
        //! \threadsafe
        virtual BlackMisc::Aviation::CAircraftSituation getInterpolatedSituation(
            const BlackMisc::Aviation::CCallsign &callsign, qint64 currentTimeSinceEpoc,
            bool isVtolAircraft, InterpolationStatus &status) const = 0;

        //! Parts before given offset time (aka pending parts)
        //! \threadsafe
        virtual BlackMisc::Aviation::CAircraftPartsList getPartsBeforeTime(
            const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTime,
            PartsStatus &partsStatus);

        //! Enable debug messages
        void enableDebugMessages(bool enabled);

        static const qint64 TimeOffsetMs = 6000; //!< offset for interpolation

    protected:
        //! Constructor
        IInterpolator(BlackMisc::Simulation::IRemoteAircraftProvider *provider, const QString &objectName, QObject *parent);

        bool m_withDebugMsg = false;  //!< allows to disable debug messages
    };

} // namespace

#endif // guard
