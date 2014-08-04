/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SETTINGS_KEYBOARDHOTKEY_H
#define BLACKMISC_SETTINGS_KEYBOARDHOTKEY_H

#include "valueobject.h"
#include "hwkeyboardkey.h"
#include "hotkeyfunction.h"
#include <QStringList>
#include <QKeySequence>

namespace BlackMisc
{
    namespace Settings
    {
        //! Value object encapsulating the keyboard hotkey assignment
        class CSettingKeyboardHotkey : public CValueObject
        {
        public:

            //! Properties by index
            enum ColumnIndex
            {
                IndexKey = 0,
                IndexKeyAsString,
                IndexKeyAsStringRepresentation,
                IndexModifier1,
                IndexModifier2,
                IndexModifier1AsString,
                IndexModifier2AsString,
                IndexFunction,
                IndexFunctionAsString,
                IndexObject, // just for updates
            };

            //! Default constructor
            CSettingKeyboardHotkey() {}

            //! Constructor
            CSettingKeyboardHotkey(const CHotkeyFunction &function);

            //! Constructor
            CSettingKeyboardHotkey(const Hardware::CKeyboardKey &key, const CHotkeyFunction &function);

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
            bool operator ==(const CSettingKeyboardHotkey &other) const;

            //! Unequal operator !=
            bool operator !=(const CSettingKeyboardHotkey &other) const;

            //! <
            bool operator<(CSettingKeyboardHotkey const &other) const;

            //! Get key code
            const Hardware::CKeyboardKey &getKey() const { return m_key; }

            //! Set key code
            void setKey(const Hardware::CKeyboardKey &key);

            //! Function
            CHotkeyFunction getFunction() const { return m_hotkeyFunction; }

            //! Set function
            void setFunction(const CHotkeyFunction &function) { this->m_hotkeyFunction = function; }

            //! Set object
            void setObject(const CSettingKeyboardHotkey &obj);

            //! Cleanup key
            void cleanup() { m_key.cleanup(); }

            //! Get modifier1 as string
            QString getModifier1AsString() const { return m_key.getModifier1AsString(); }

            //! Get modifier2 as string
            QString getModifier2AsString() const { return m_key.getModifier1AsString(); }

            //! Modifiers
            static QStringList modifiers() { return Hardware::CKeyboardKey::modifiers(); }

            //! Create string representation
            static QString toStringRepresentation(int key) { return Hardware::CKeyboardKey::toStringRepresentation(key); }

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const QVariant &variant, int index);

            //! \copydoc CValueObject::propertyByIndex
            virtual QVariant propertyByIndex(int index) const;

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
            BLACK_ENABLE_TUPLE_CONVERSION(CSettingKeyboardHotkey)
            Hardware::CKeyboardKey m_key; //!< code similar to Qt::Key
            CHotkeyFunction m_hotkeyFunction; //!< hotkey function

        };
    } // class
} // BlackMisc

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Settings::CSettingKeyboardHotkey, (o.m_key, o.m_hotkeyFunction))
Q_DECLARE_METATYPE(BlackMisc::Settings::CSettingKeyboardHotkey)

#endif // guard
