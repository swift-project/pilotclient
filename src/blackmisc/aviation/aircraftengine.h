/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AIRCRAFTENGINES_H
#define BLACKMISC_AIRCRAFTENGINES_H

#include "blackmisc/valueobject.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating information about aircraft's engines
        class CAircraftEngine : public CValueObjectStdTuple<CAircraftEngine>
        {
        public:

            //! Default constructor
            CAircraftEngine() = default;

            //! Constructor
            CAircraftEngine(int number, bool on) : m_number(number), m_on(on) {}

            //! Get engine number
            int getNumber() const { return m_number; }

            //! Set engine number
            void setNumber (int number) { m_number = number; }

            //! Is on/off?
            bool isOn() const { return m_on; }

            //! Set to on/off
            void setOn(bool on) { m_on = on; }

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftEngine)
            int m_number = 1;
            bool m_on = true;

        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAircraftEngine, (
                                   attr(o.m_number, flags<DisabledForJson>()),
                                   attr(o.m_on, "on")
                               ))

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftEngine)

#endif // BLACKMISC_AIRCRAFTENGINES_H
