/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_EVENT_HOTKEYFUNCTION_H
#define BLACKMISC_EVENT_HOTKEYFUNCTION_H

//! \file

#include "blackmiscexport.h"
#include "valueobject.h"
#include "originator.h"
#include "hotkeyfunction.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Event
    {
        //! Value object encapsulating a hotkey function for distribution
        class BLACKMISC_EXPORT CEventHotkeyFunction : public CValueObject<CEventHotkeyFunction>
        {
        public:
            //! Default constructor.
            CEventHotkeyFunction() = default;

            //! Constructor.
            CEventHotkeyFunction(CHotkeyFunction func, bool argument);

            //! Get the event originator
            const COriginator &getEventOriginator() const {return m_eventOriginator;}

            //! Get the event key
            const BlackMisc::CHotkeyFunction &getFunction() const {return m_hotkeyFunc;}

            //! Get boolean hotkey function argument
            bool getFunctionArgument() const { return m_hotkeyFuncArgument; }

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CEventHotkeyFunction)
            COriginator m_eventOriginator;
            CHotkeyFunction m_hotkeyFunc;

            // This is the required argument to call a registered function per CHotkeyFunction
            bool m_hotkeyFuncArgument = false;
        };
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Event::CEventHotkeyFunction, (attr(o.m_eventOriginator, flags<DisabledForJson>()), o.m_hotkeyFunc))
Q_DECLARE_METATYPE(BlackMisc::Event::CEventHotkeyFunction)

#endif // BLACKMISC_EVENTHOTKEY_H
