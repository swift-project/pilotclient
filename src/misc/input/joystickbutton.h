// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_INPUT_JOYSTICKBUTTON_H
#define SWIFT_MISC_INPUT_JOYSTICKBUTTON_H

#include "misc/swiftmiscexport.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/valueobject.h"

#include <QMetaType>
#include <QString>
#include <QtGlobal>

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::input, CJoystickButton)

namespace swift::misc::input
{
    //! Value object representing a joystick button
    class SWIFT_MISC_EXPORT CJoystickButton : public CValueObject<CJoystickButton>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexDeviceName = CPropertyIndexRef::GlobalIndexCJoystickButton,
            IndexButton,
            IndexButtonAsString,
            IndexButtonObject, // just for updates
        };

        //! Default constructor
        CJoystickButton() = default;

        //! Constructor
        CJoystickButton(const QString &deviceName, int index);

        //! Get device name
        QString getDeviceName() const { return m_deviceName; }

        //! Get button index
        int getButtonIndex() const { return m_buttonIndex; }

        //! Get button as String
        QString getButtonAsString() const { return buttonIndexToString(m_buttonIndex); }

        //! Get button as String including its device name
        QString getButtonAsStringWithDeviceName() const;

        //! Set owning device name
        void setDeviceName(const QString &deviceName) { m_deviceName = deviceName; }

        //! Set button index
        void setButtonIndex(int buttonIndex);

        //! Is valid?
        bool isValid() const;

        //! Set button object
        void setButtonObject(CJoystickButton button);

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! Button index to string
        static QString buttonIndexToString(qint32 buttonIndex);

        //! Button index from string
        static int buttonIndexFromString(const QString &button);

        //! Invalid button index
        static int getInvalidIndex() { return InvalidButtonIndex; }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        static const QString &buttonIndentifier()
        {
            static const QString bn("Button");
            return bn;
        }
        static constexpr int InvalidButtonIndex = -1;

        QString m_deviceName;
        int m_buttonIndex = InvalidButtonIndex;

        SWIFT_METACLASS(
            CJoystickButton,
            SWIFT_METAMEMBER(deviceName),
            SWIFT_METAMEMBER(buttonIndex));
    };
} // ns

Q_DECLARE_METATYPE(swift::misc::input::CJoystickButton)

#endif // guard
