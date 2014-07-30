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

        /*
         * Hash
         */
        uint CJoystickButton::getValueHash() const
        {
            return qHash(TupleConverter<CJoystickButton>::toMetaTuple(*this));
        }

        // To JSON
        QJsonObject CJoystickButton::toJson() const
        {
            return BlackMisc::serializeJson(TupleConverter<CJoystickButton>::toMetaTuple(*this));
        }

        // From Json
        void CJoystickButton::convertFromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, TupleConverter<CJoystickButton>::toMetaTuple(*this));
        }

        // Meta data
        void CJoystickButton::registerMetadata()
        {
            qRegisterMetaType<CJoystickButton>();
            qDBusRegisterMetaType<CJoystickButton>();
        }

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

        /*
         * Equal?
         */
        bool CJoystickButton::operator ==(const CJoystickButton &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CJoystickButton>::toMetaTuple(*this) == TupleConverter<CJoystickButton>::toMetaTuple(other);
        }

        /*
         * Unequal?
         */
        bool CJoystickButton::operator !=(const CJoystickButton &other) const
        {
            return !((*this) == other);
        }

        bool CJoystickButton::operator< (CJoystickButton const &other) const
        {
            return TupleConverter<CJoystickButton>::toMetaTuple(*this) < TupleConverter<CJoystickButton>::toMetaTuple(other);
        }

        QString CJoystickButton::convertToQString(bool /* i18n */) const
        {
            QString s = getButtonAsString();
            return s.trimmed();
        }

        int CJoystickButton::getMetaTypeId() const
        {
            return qMetaTypeId<CJoystickButton>();
        }

        bool CJoystickButton::isA(int metaTypeId) const
        {
            return (metaTypeId == qMetaTypeId<CJoystickButton>());
        }

        /*
         * Compare
         */
        int CJoystickButton::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CJoystickButton &>(otherBase);
            return compare(TupleConverter<CJoystickButton>::toMetaTuple(*this), TupleConverter<CJoystickButton>::toMetaTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CJoystickButton::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CJoystickButton>::toMetaTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CJoystickButton::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CJoystickButton>::toMetaTuple(*this);
        }

    } // namespace Hardware

} // BlackMisc
