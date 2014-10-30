/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "hwjoystickbutton.h"
#include "blackmiscfreefunctions.h"
#include <QCoreApplication>

namespace BlackMisc
{
    namespace Hardware
    {
        CJoystickButton::CJoystickButton(qint32 buttonIndex) :
            m_buttonIndex(buttonIndex)
        {}

        void CJoystickButton::setButtonIndex(qint32 buttonIndex)
        {
            m_buttonIndex = buttonIndex;
        }

        void CJoystickButton::setButtonObject(const CJoystickButton &button)
        {
            this->m_buttonIndex = button.m_buttonIndex;
        }

        void CJoystickButton::setPropertyByIndex(const QVariant &variant, const CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromQVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();

            switch (i)
            {
            case IndexButton:
            case IndexButtonAsString:
                this->setButtonIndex(buttonIndexFromString(variant.value<QString>()));
                break;
            case IndeButtonObject:
                this->setButtonObject(variant.value<BlackMisc::Hardware::CJoystickButton>());
                break;
            default:
                Q_ASSERT_X(false, "CJoystickButton", "index unknown (setter)");
                break;
            }
        }

        QVariant CJoystickButton::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexButton:
                return QVariant(this->getButtonIndex());
            case IndexButtonAsString:
                return QVariant(this->getButtonAsString());
            default:
                break;
            }

            Q_ASSERT_X(false, "CJoystickButton", "index unknown");
            QString m = QString("no property, index ").append(index.toQString());
            return QVariant::fromValue(m);
        }

        QString CJoystickButton::buttonIndexToString(qint32 buttonIndex)
        {
            QString buttonString("Button");
            return buttonString.append(QString("%1").arg(buttonIndex));
        }

        qint32 CJoystickButton::buttonIndexFromString(const QString &buttonName)
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
