/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_JOYSTICKBUTTON_H
#define BLACKMISC_JOYSTICKBUTTON_H

#include "valueobject.h"
#include "propertyindex.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Hardware
    {
        //! Value object representing a joystick button
        class CJoystickButton : public CValueObjectStdTuple<CJoystickButton>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexButton = 0,
                IndexButtonAsString,
                IndeButtonObject, // just for updates
            };

            //! Default constructor
            CJoystickButton() = default;

            //! Constructor
            CJoystickButton(qint32 buttonIndex);

            //! Get button index
            qint32 getButtonIndex() const { return m_buttonIndex; }

            //! Get button as String
            QString getButtonAsString() const { return buttonIndexToString(m_buttonIndex); }

            //! Set button index
            void setButtonIndex(qint32 buttonIndex);

            //! Is valid?
            bool isValid() const { return m_buttonIndex >= 0 ? true : false; }

            //! Set button object
            void setButtonObject(const BlackMisc::Hardware::CJoystickButton &button);

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc CValueObject::propertyByIndex
            virtual QVariant propertyByIndex(const CPropertyIndex &index) const;

            //! Button index to string
            static QString buttonIndexToString(qint32 buttonIndex);

            //! Button index from string
            static qint32 buttonIndexFromString(const QString &button);

            //! Invalid button index
            static qint32 getInvalidIndex() { return m_invalidIndex; }

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CJoystickButton)
            qint32 m_buttonIndex = m_invalidIndex; //!< code similar to Qt::Key

            static const qint32 m_invalidIndex = -1;
        };
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Hardware::CJoystickButton, (o.m_buttonIndex))
Q_DECLARE_METATYPE(BlackMisc::Hardware::CJoystickButton)

#endif // guard
