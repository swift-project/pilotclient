/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_AVAIRCRAFTSITUATIONLIST_H
#define BLACKMISC_AVAIRCRAFTSITUATIONLIST_H

#include "avaircraftsituation.h"
#include "sequence.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! \brief Value object encapsulating a list of aircraft situations
        class CAircraftSituationList : public CSequence<CAircraftSituation>
        {
        public:
            //! \brief Default constructor.
            CAircraftSituationList();

            //! \brief Construct from a base class object.
            CAircraftSituationList(const CSequence<CAircraftSituation> &other);

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Get a list of situations before dateTime
             * \param dateTime
             * \return
             */
            CAircraftSituationList findBefore (const QDateTime& dateTime) const;

            /*!
             * \brief Get a list of situations after dateTime
             * \param dateTime
             * \return
             */
            CAircraftSituationList findAfter (const QDateTime& dateTime) const;

            /*!
             * \brief Remove situations with timestamp before dateTime
             * \param dateTime
             */
            void removeBefore(const QDateTime& dateTime);

            /*!
             * \brief Remove situations older than seconds
             * \param seconds
             */
            void removeOlderThan(double seconds);
        };
    }
}

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationList)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraftSituation>)

#endif // guard
