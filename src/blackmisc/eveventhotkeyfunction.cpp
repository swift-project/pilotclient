/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "eveventhotkeyfunction.h"

namespace BlackMisc
{
    namespace Event
    {
        CEventHotkeyFunction::CEventHotkeyFunction()
        {

        }

        CEventHotkeyFunction::CEventHotkeyFunction(CHotkeyFunction func, bool argument)
            : m_hotkeyFunc(func), m_hotkeyFuncArgument(argument)
        {
        }

        // Hash
        uint CEventHotkeyFunction::getValueHash() const
        {
            return qHash(TupleConverter<CEventHotkeyFunction>::toMetaTuple(*this));
        }

        // Register metadata
        void CEventHotkeyFunction::registerMetadata()
        {
            qRegisterMetaType<CEventHotkeyFunction>();
            qDBusRegisterMetaType<CEventHotkeyFunction>();
        }

        /*
         * Convert to string
         */
        QString CEventHotkeyFunction::convertToQString(bool i18n) const
        {
            QString s;
            s.append(m_eventOriginator.toQString(i18n));
            s.append(" ").append(m_hotkeyFunc.toQString(i18n));
            return s;
        }

        /*
         * metaTypeId
         */
        int CEventHotkeyFunction::getMetaTypeId() const
        {
            return qMetaTypeId<CEventHotkeyFunction>();
        }

        /*
         * is a
         */
        bool CEventHotkeyFunction::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CEventHotkeyFunction>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CEventHotkeyFunction::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CEventHotkeyFunction &>(otherBase);
            return compare(TupleConverter<CEventHotkeyFunction>::toMetaTuple(*this), TupleConverter<CEventHotkeyFunction>::toMetaTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CEventHotkeyFunction::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CEventHotkeyFunction>::toMetaTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CEventHotkeyFunction::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CEventHotkeyFunction>::toMetaTuple(*this);
        }
    }
}
