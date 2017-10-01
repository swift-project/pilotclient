/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_FLIGHTPLANUTILS_H
#define BLACKMISC_AVIATION_FLIGHTPLANUTILS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/network/voicecapabilities.h"
#include "callsign.h"
#include <QString>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Flight plan utils
        class BLACKMISC_EXPORT CFlightPlanUtils
        {
        public:
            //! Useful values in flight plan remarks
            struct BLACKMISC_EXPORT FlightPlanRemarks
            {
                QString radioTelephony; //!< radio telephony designator
                QString flightOperator; //!< operator, i.e. normally the airline name
                QString airlineIcao;    //!< airline ICAO as provided in flightplan
                QString selcal;         //!< SELCAL
                CCallsign callsign;     //!< callsign of other pilot
                bool isNull = true;     //!< marked as NULL
                Network::CVoiceCapabilities voiceCapabilities; //!< voice capabilities

                //! Any remarks available?
                bool hasAnyRemarks() const;

                //! Airline remarks
                bool hasAirlineRemarks() const;

                //! Valid airline ICAO?
                //! \remark valid here means valid syntax, no guarantee it really exists
                bool hasValidAirlineIcao() const;
            };

            //! Constructor
            CFlightPlanUtils() = delete;

            //! Parse remarks from a flight plan
            static FlightPlanRemarks parseFlightPlanRemarks(const QString &remarks, const Network::CVoiceCapabilities voiceCapabilities = {});

            //! Get aircraft ICAO code from equipment code like
            //! \remark we expect something like H/B772/F B773 B773/F
            static QString aircraftIcaoCodeFromEquipmentCode(const QString &equipmentCodeAndAircraft);

        private:
            //! Cut the remarks part
            static QString cut(const QString &remarks, const QString &marker);
        };
    } // namespace
} // namespace

#endif // guard
