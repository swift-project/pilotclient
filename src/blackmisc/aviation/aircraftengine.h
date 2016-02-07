/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTENGINES_H
#define BLACKMISC_AVIATION_AIRCRAFTENGINES_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating information about aircraft's engines
        class BLACKMISC_EXPORT CAircraftEngine : public CValueObject<CAircraftEngine>
        {
        public:

            //! Default constructor
            CAircraftEngine() {}

            //! Constructor
            CAircraftEngine(int number, bool on);

            //! Get engine number
            int getNumber() const { return m_number; }

            //! Set engine number
            void setNumber (int number);

            //! Is on/off?
            bool isOn() const { return m_on; }

            //! Set to on/off
            void setOn(bool on) { m_on = on; }

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftEngine)
            int m_number = 1;
            bool m_on = true;

        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftEngine)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAircraftEngine, (
                                   attr(o.m_number, flags<DisabledForJson>()),
                                   attr(o.m_on, "on")
                               ))

#endif // guard
