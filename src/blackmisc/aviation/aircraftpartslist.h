/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTPARTSLIST_H
#define BLACKMISC_AVIATION_AIRCRAFTPARTSLIST_H

#include "aircraftparts.h"
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/aviation/callsignobjectlist.h"
#include <QDateTime>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating a list of aircraft parts.
        class CAircraftPartsList :
            public CSequence<CAircraftParts>,
            public ITimestampObjectList<CAircraftParts, CAircraftPartsList>,
            public ICallsignObjectList<CAircraftParts, CAircraftPartsList>
        {
        public:
            //! Default constructor.
            CAircraftPartsList();

            //! Construct from a base class object.
            CAircraftPartsList(const CSequence<CAircraftParts> &other);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Register metadata
            static void registerMetadata();

        protected:
            //! Myself
            virtual const CAircraftPartsList &container() const { return *this; }

            //! Myself
            virtual CAircraftPartsList &container() { return *this; }

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftPartsList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftParts>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraftParts>)

#endif //guard
