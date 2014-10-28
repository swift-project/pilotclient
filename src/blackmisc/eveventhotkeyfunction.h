/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_EVENT_HOTKEYFUNCTION_H
#define BLACKMISC_EVENT_HOTKEYFUNCTION_H

//! \file

#include "valueobject.h"
#include "evoriginator.h"
#include "hotkeyfunction.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Event
    {
        //! Value object encapsulating a hotkey function for distribution
        class CEventHotkeyFunction : public BlackMisc::CValueObject
        {
        public:
            //! Default constructor.
            CEventHotkeyFunction() = default;

            //! Constructor.
            CEventHotkeyFunction(CHotkeyFunction func, bool argument);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! Register metadata
            static void registerMetadata();

            //! Get the event originator
            const COriginator &getEventOriginator() const {return m_eventOriginator;}

            //! Get the event key
            const BlackMisc::CHotkeyFunction &getFunction() const {return m_hotkeyFunc;}

            //! Get boolean hotkey function argument
            bool getFunctionArgument() const { return m_hotkeyFuncArgument; }

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CEventHotkeyFunction)
            COriginator m_eventOriginator;
            CHotkeyFunction m_hotkeyFunc;

            // This is the required argument to call a registered function per CHotkeyFunction
            bool m_hotkeyFuncArgument = false;
        };
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Event::CEventHotkeyFunction, (o.m_eventOriginator, o.m_hotkeyFunc))
Q_DECLARE_METATYPE(BlackMisc::Event::CEventHotkeyFunction)

#endif // BLACKMISC_EVENTHOTKEY_H
