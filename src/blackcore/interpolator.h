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

#include "blackmisc/avaircraftsituation.h"
#include "blackmisc/simulation/simdirectaccessremoteaircraft.h"
#include "blackmisc/worker.h"
#include "simulator.h"
#include <QHash>
#include <QList>

namespace BlackCore
{
    //! Interpolator, calculation inbetween positions
    class IInterpolator :
        public BlackMisc::CContinuousWorker,
        public BlackMisc::Simulation::CRemoteAircraftProviderSupportReadOnly
    {
        Q_OBJECT

    public:
        //! Virtual destructor
        virtual ~IInterpolator() {}

        //! Log category
        static QString getMessageCategory() { return "swift.iinterpolator"; }

        //! Has situations?
        //! \deprecated Try not to use, it would be more efficient to directly getting the values and decide then
        //! \threadsafe
        virtual bool hasEnoughAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Current interpolated situation
        //! \threadsafe
        virtual BlackMisc::Aviation::CAircraftSituation getCurrentInterpolatedSituation(const QHash<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituationList> &allSituations, const BlackMisc::Aviation::CCallsign &callsign, qint64 currentTimeSinceEpoc = -1, bool *ok = nullptr) const = 0;

        //! Latest parts before time - offset
        //! \threadsafe
        BlackMisc::Aviation::CAircraftParts getLatestPartsBeforeOffset(const BlackMisc::Aviation::CCallsign &callsign, qint64 timeOffset = TimeOffsetMs, bool *ok = nullptr) const;

        //! The situations per callsign
        //! \threadsafe
        QHash<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituationList> getSituationsByCallsign() const;

        //! Situations for given callsign
        //! \threadsafe
        BlackMisc::Aviation::CAircraftSituationList getSituationsForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Last finished request id, -1 means none
        //! \threadsafe
        int latestFinishedRequestId() const;

        //! Latest calculation
        //! \threadsafe
        BlackMisc::Aviation::CAircraftSituationList latestFinishedRequest() const;

        //! Calculation by id
        //! \threadsafe
        BlackMisc::Aviation::CAircraftSituationList getRequest(int requestId, bool *ok = nullptr) const;

        //! Enable debug messages
        void enableDebugMessages(bool enabled);

        static const qint64 TimeOffsetMs = 6000;           //!< offset for interpolation
        static const int MaxSituationsPerCallsign = 6;     //!< How many situations per callsign
        static const int MaxPartsPerCallsign = 3;          //!< How many parts per callsign
        static const int MaxKeptInterpolationRequests = 3; //!< How many requests are stored

    public slots:
        //! Do a complete calculation for all know callsigns in background.
        //! Only use positive numbers.
        //! \param currentTimeMsSinceEpoch if no value is passed current time is used
        //! \threadsafe
        void syncRequestSituationsCalculationsForAllCallsigns(int requestId, qint64 currentTimeMsSinceEpoch = -1);

        //! Do a complete calculation for all know callsigns in background.
        //! Non blocking call of \syncRequestSituationsCalculationsForAllCallsigns
        //! \threadsafe
        void asyncRequestSituationsCalculationsForAllCallsigns(int requestId, qint64 currentTimeMsSinceEpoch = -1);

    private slots:
        //! New situation got added
        //! \threadsafe
        void ps_onAddedAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Added aircraft parts
        //! \threadsafe
        void ps_onAddedAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts);

        //! Removed aircraft
        //! \threadsafe
        void ps_onRemovedAircraft(const BlackMisc::Aviation::CCallsign &callsign);

    protected:
        //! Constructor
        IInterpolator(BlackMisc::Simulation::IRemoteAircraftProviderReadOnly *provider, const QString &workerName, QObject *parent = nullptr);

        //! Situations for times before / after
        //! \sa ITimestampObjectList::splitByTime
        //! \threadsafe
        //! \deprecated For first version
        QList<BlackMisc::Aviation::CAircraftSituationList> getSituationsTimeSplit(const BlackMisc::Aviation::CCallsign &callsign, qint64 splitTimeMsSinceEpoch) const;

        bool m_withDebugMsg = false; //!< allows to disable debug messages

    private:
        mutable QReadWriteLock m_situationsLock;
        mutable QReadWriteLock m_partsLock;
        mutable QReadWriteLock m_requestedInterpolationsLock;

        // hashs, because not sorted by key but keeping order
        QHash<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituationList> m_situationsByCallsign;
        QHash<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftPartsList> m_partsByCallsign;
        QHash<int, BlackMisc::Aviation::CAircraftSituationList> m_requestedInterpolations;
    };

} // namespace

#endif // guard
