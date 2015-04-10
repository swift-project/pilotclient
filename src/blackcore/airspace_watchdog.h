/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AIRSPACE_WATCHDOG_H
#define BLACKCORE_AIRSPACE_WATCHDOG_H

#include "blackcoreexport.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/aviation/callsign.h"
#include <QObject>
#include <QHash>

namespace BlackCore
{

    /*!
     * CAirspaceWatchdog monitores any added callsign, if it has timed out.
     *
     * This class was introduced due to a flaw in the VATSIM server implementation:
     * Every client needs to send an add/delete packet on its own to inform other
     * clients nearby. The server does not take care of that. When a client crashes,
     * no delete packet is ever sent. This class therefore monitors callsigns and
     * emits a timeout signal if it wasn't resetted during the specified timeout
     * value.
     */
    class BLACKCORE_EXPORT CAirspaceWatchdog : public QObject
    {
        Q_OBJECT

    public:
        //! Default constructor
        CAirspaceWatchdog(QObject *parent);

        //! Constructor
        CAirspaceWatchdog(const BlackMisc::PhysicalQuantities::CTime & initialTimeOut, QObject *parent);

        //! Sets the maximum time, after a client is considered timed out
        void setTimeout(const BlackMisc::PhysicalQuantities::CTime &value) { m_timeout = value; }

        //! Returns the currently configured timeout value
        const BlackMisc::PhysicalQuantities::CTime &getTimeout() const { return m_timeout; }

        //! Add a new callsign to watch list
        void addCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Reset the watchdog for Callsign
        //! The callsign must have been added by \sa addCallsign before.
        void resetCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Remove Callsign from watch list
        void removeCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Remove all watched callsigns
        void removeAll();

    signals:
        //! Callsign has timed out
        void timeout(const BlackMisc::Aviation::CCallsign &callsign);

    protected:
        //! \overload QObject::timerEvent
        virtual void timerEvent(QTimerEvent * event) override;

    private:

        void checkTimeouts();

        BlackMisc::PhysicalQuantities::CTime m_timeout = {15, BlackMisc::PhysicalQuantities::CTimeUnit::s() }; //!< Timeout value
        QHash<BlackMisc::Aviation::CCallsign, QDateTime> m_callsignTimestamps; //!< Hash of callsigns and latest timestamp

    };

} // namespace

#endif
