// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_INPUT_KEYBOARDKEY_H
#define SWIFT_MISC_INPUT_KEYBOARDKEY_H

#include "misc/swiftmiscexport.h"
#include "misc/input/keycodes.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/valueobject.h"

#include <QList>
#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc::input, CKeyboardKey)

namespace swift::misc::input
{
    //! Value object representing a keyboard key.
    class SWIFT_MISC_EXPORT CKeyboardKey : public CValueObject<CKeyboardKey>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexKey = CPropertyIndexRef::GlobalIndexCKeyboardKey,
            IndexKeyAsString,
            IndexKeyObject, // just for updates
        };

        //! Default constructor
        CKeyboardKey();

        //! Constructor
        CKeyboardKey(swift::misc::input::KeyCode keyCode);

        //! Get key code
        KeyCode getKey() const { return m_keyCode; }

        //! Get key code
        QString getKeyAsString() const;

        //! Set key code
        void setKey(KeyCode key) { m_keyCode = key; }

        //! Set key code
        void setKey(int key) { m_keyCode = static_cast<KeyCode>(key); }

        //! Set key code
        void setKey(char key) { m_keyCode = static_cast<KeyCode>(key); }

        //! Is unknown?
        bool isUnknown() const { return !this->hasKey(); }

        //! Modifier?
        bool isModifier() const
        {
            return allModifiers().contains(m_keyCode);
        }

        //! with key?
        bool hasKey() const
        {
            return !(m_keyCode == KeyCode::Key_Unknown);
        }

        //! Set key object
        void setKeyObject(CKeyboardKey key);

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        static const QList<KeyCode> &allModifiers();

        KeyCode m_keyCode; //!< Key code

        BLACK_METACLASS(
            CKeyboardKey,
            BLACK_METAMEMBER(keyCode)
        );
    };
} // ns

Q_DECLARE_METATYPE(swift::misc::input::CKeyboardKey)

#endif // guard
