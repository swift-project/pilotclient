/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SETTINGS_KEYBOARDHOTKEY_H
#define BLACKMISC_SETTINGS_KEYBOARDHOTKEY_H

#include "propertyindex.h"
#include "hwkeyboardkey.h"
#include "hotkeyfunction.h"
#include "blackmiscfreefunctions.h"
#include <QStringList>
#include <QKeySequence>

namespace BlackMisc
{
    namespace Settings
    {
        //! Value object encapsulating the keyboard hotkey assignment
        class CSettingKeyboardHotkey : public CValueObjectStdTuple<CSettingKeyboardHotkey>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexKey = BlackMisc::CPropertyIndex::GlobalIndexCSettingKeyboardHotkey,
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
            CSettingKeyboardHotkey() = default;

            //! Constructor
            CSettingKeyboardHotkey(const CHotkeyFunction &function);

            //! Constructor
            CSettingKeyboardHotkey(const Hardware::CKeyboardKey &key, const CHotkeyFunction &function);

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
            virtual void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! \copydoc CValueObject::propertyByIndex
            virtual QVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CSettingKeyboardHotkey)
            Hardware::CKeyboardKey m_key; //!< code similar to Qt::Key
            CHotkeyFunction m_hotkeyFunction; //!< hotkey function
        };
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Settings::CSettingKeyboardHotkey, (o.m_key, o.m_hotkeyFunction))
Q_DECLARE_METATYPE(BlackMisc::Settings::CSettingKeyboardHotkey)

#endif // guard
