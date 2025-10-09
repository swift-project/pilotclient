// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/input/joystickbutton.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::input, CJoystickButton)

namespace swift::misc::input
{
    CJoystickButton::CJoystickButton(const QString &deviceName, int index)
        : m_deviceName(deviceName), m_buttonIndex(index)
    {}

    QString CJoystickButton::getButtonAsStringWithDeviceName() const
    {
        return QStringLiteral("%1%2 - %3").arg(buttonIndentifier()).arg(m_buttonIndex).arg(m_deviceName);
    }

    void CJoystickButton::setButtonIndex(int buttonIndex) { m_buttonIndex = buttonIndex; }

    bool CJoystickButton::isValid() const { return (!m_deviceName.isEmpty() && m_buttonIndex >= 0); }

    void CJoystickButton::setButtonObject(CJoystickButton button)
    {
        m_deviceName = button.m_deviceName;
        m_buttonIndex = button.m_buttonIndex;
    }

    void CJoystickButton::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CJoystickButton>();
            return;
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDeviceName: this->setDeviceName(variant.value<QString>()); break;
        case IndexButton:
        case IndexButtonAsString: this->setButtonIndex(buttonIndexFromString(variant.value<QString>())); break;
        case IndexButtonObject: this->setButtonObject(variant.value<swift::misc::input::CJoystickButton>()); break;
        default: Q_ASSERT_X(false, "CJoystickButton", "index unknown (setter)"); break;
        }
    }

    QVariant CJoystickButton::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDeviceName: return QVariant::fromValue(this->getDeviceName());
        case IndexButton: return QVariant::fromValue(this->getButtonIndex());
        case IndexButtonAsString: return QVariant::fromValue(this->getButtonAsString());
        case IndexButtonObject: return QVariant::fromValue(*this);
        }

        Q_ASSERT_X(false, "CJoystickButton", "index unknown");
        QString m = QString("no property, index ").append(index.toQString());
        return QVariant::fromValue(m);
    }

    QString CJoystickButton::buttonIndexToString(qint32 buttonIndex)
    {
        return buttonIndentifier() + QString::number(buttonIndex);
    }

    int CJoystickButton::buttonIndexFromString(const QString &buttonName)
    {
        if (!buttonName.startsWith(buttonIndentifier())) { return getInvalidIndex(); }
        QString name(buttonName);
        name.remove(buttonIndentifier());
        if (name.contains('-')) { name = name.mid(0, name.indexOf('-')); }
        return name.trimmed().toInt();
    }

    QString CJoystickButton::convertToQString(bool /* i18n */) const { return getButtonAsString().trimmed(); }

} // namespace swift::misc::input
