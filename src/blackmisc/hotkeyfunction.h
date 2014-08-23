/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_HOTKEYFUNCTION_H
#define BLACKMISC_HOTKEYFUNCTION_H

#include "valueobject.h"

namespace BlackMisc
{
    //! Value object representing a hotkey function.
    class CHotkeyFunction : public CValueObject
    {
    public:

        //! Function type
        enum Function
        {
            HotkeyNone,
            HotkeyPtt,
            HotkeyToggleCom1,
            HotkeyToggleCom2,
            HotkeyOpacity50,
            HotkeyOpacity100,
            HotkeyToogleWindowsStayOnTop
        };

        //! Default constructor
        CHotkeyFunction();

        //! Constructor by function
        CHotkeyFunction(Function function);

        //! Get function as string
        QString getFunctionAsString() const;

        //! Function
        Function getFunction() const { return m_function; }

        //! Set function
        void setFunction(const Function &function) { m_function = function; }

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const override
        {
            return QVariant::fromValue(*this);
        }

        //! \copydoc CValueObject::getValueHash
        virtual uint getValueHash() const override;

        //! \copydoc CValueObject::toJson
        virtual QJsonObject toJson() const override;

        //! \copydoc CValueObject::fromJson
        void fromJson(const QJsonObject &json) override;

        //! Register metadata
        static void registerMetadata();

        //! Equal?
        bool operator ==(const CHotkeyFunction &other) const;

        //! Hotkey function is Ptt
        static const CHotkeyFunction &Ptt()
        {
            static CHotkeyFunction hotkeyFunction(HotkeyPtt);
            return hotkeyFunction;
        }

        //! Hotkey function is toggle Com1
        static const CHotkeyFunction &ToggleCom1()
        {
            static CHotkeyFunction hotkeyFunction(HotkeyToggleCom1);
            return hotkeyFunction;
        }

        //! Hotkey function is toggle Com2
        static const CHotkeyFunction &ToggleCom2()
        {
            static CHotkeyFunction hotkeyFunction(HotkeyToggleCom2);
            return hotkeyFunction;
        }

        //! Hotkey function is opacity 50
        static const CHotkeyFunction &Opacity50()
        {
            static CHotkeyFunction hotkeyFunction(HotkeyOpacity50);
            return hotkeyFunction;
        }

        //! Hotkey function is opacity 100
        static const CHotkeyFunction &Opacity100()
        {
            static CHotkeyFunction hotkeyFunction(HotkeyOpacity100);
            return hotkeyFunction;
        }

        //! Hotkey function is toggle windows on top
        static const CHotkeyFunction &ToogleWindowsStayOnTop()
        {
            static CHotkeyFunction hotkeyFunction(HotkeyToogleWindowsStayOnTop);
            return hotkeyFunction;
        }

    protected:

        //! \copydoc CValueObject::convertToQString
        virtual QString convertToQString(bool i18n = false) const override;

        //! \copydoc CValueObject::getMetaTypeId
        virtual int getMetaTypeId() const override;

        //! \copydoc CValueObject::isA
        virtual bool isA(int metaTypeId) const override;

        //! \copydoc CValueObject::compareImpl(otherBase)
        virtual int compareImpl(const CValueObject &otherBase) const override;

        //! \copydoc CValueObject::marshallToDbus
        virtual void marshallToDbus(QDBusArgument &argument) const override;

        //! \copydoc CValueObject::unmarshallFromDbus
        virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CHotkeyFunction)
        Function m_function;
    };
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CHotkeyFunction, (o.m_function))
Q_DECLARE_METATYPE(BlackMisc::CHotkeyFunction)

#endif //BLACKMISC_HOTKEYFUNCTION_H
