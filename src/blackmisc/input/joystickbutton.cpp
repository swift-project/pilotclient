/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/input/joystickbutton.h"

#include "blackmisc/variant.h"

namespace BlackMisc
{
    namespace Input
    {
        CJoystickButton::CJoystickButton(const QString &deviceName, int index) :
            m_deviceName(deviceName), m_buttonIndex(index)
        {}

        void CJoystickButton::setButtonIndex(int buttonIndex)
        {
            m_buttonIndex = buttonIndex;
        }

        bool CJoystickButton::isValid() const
        {
            if (!m_deviceName.isEmpty() && m_buttonIndex >= 0)  { return true; }
            else { return false; }
        }

        void CJoystickButton::setButtonObject(CJoystickButton button)
        {
            m_deviceName = button.m_deviceName;
            m_buttonIndex = button.m_buttonIndex;
        }

        void CJoystickButton::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CJoystickButton>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDeviceName:
                this->setDeviceName(variant.value<QString>());
                break;
            case IndexButton:
            case IndexButtonAsString:
                this->setButtonIndex(buttonIndexFromString(variant.value<QString>()));
                break;
            case IndexButtonObject:
                this->setButtonObject(variant.value<BlackMisc::Input::CJoystickButton>());
                break;
            default:
                Q_ASSERT_X(false, "CJoystickButton", "index unknown (setter)");
                break;
            }
        }

        CVariant CJoystickButton::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDeviceName: return CVariant::from(this->getDeviceName());
            case IndexButton: return CVariant::from(this->getButtonIndex());
            case IndexButtonAsString: return CVariant::from(this->getButtonAsString());
            case IndexButtonObject: return CVariant::from(*this);
            }

            Q_ASSERT_X(false, "CJoystickButton", "index unknown");
            QString m = QString("no property, index ").append(index.toQString());
            return CVariant::fromValue(m);
        }

        QString CJoystickButton::buttonIndexToString(qint32 buttonIndex)
        {
            QString buttonString("Button");
            return buttonString.append(QStringLiteral("%1").arg(buttonIndex));
        }

        int CJoystickButton::buttonIndexFromString(const QString &buttonName)
        {
            QString name("Button");
            if (!buttonName.startsWith(name)) return getInvalidIndex();

            name.remove("Button");
            return name.toInt();
        }

        QString CJoystickButton::convertToQString(bool /* i18n */) const
        {
            QString s = getButtonAsString();
            return s.trimmed();
        }

    } // namespace Hardware

} // BlackMisc
