/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "setkeyboardhotkey.h"
#include "blackmiscfreefunctions.h"
#include <QCoreApplication>

namespace BlackMisc
{
    namespace Settings
    {
        CSettingKeyboardHotkey::CSettingKeyboardHotkey(const CHotkeyFunction &function) :
            m_hotkeyFunction(function)
        {}

        CSettingKeyboardHotkey::CSettingKeyboardHotkey(const Hardware::CKeyboardKey &key, const CHotkeyFunction &function) :
            m_key(key), m_hotkeyFunction(function)
        {}

        void CSettingKeyboardHotkey::registerMetadata()
        {
            qRegisterMetaType<CSettingKeyboardHotkey>();
            qDBusRegisterMetaType<CSettingKeyboardHotkey>();
        }

        /*
         * To JSON
         */
        QJsonObject CSettingKeyboardHotkey::toJson() const
        {
            return BlackMisc::serializeJson(TupleConverter<CSettingKeyboardHotkey>::toMetaTuple(*this));
        }

        /*
         * From Json
         */
        void CSettingKeyboardHotkey::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, TupleConverter<CSettingKeyboardHotkey>::toMetaTuple(*this));
        }


        QString CSettingKeyboardHotkey::convertToQString(bool /* i18n */) const
        {
            return m_key.toQString();
        }

        int CSettingKeyboardHotkey::getMetaTypeId() const
        {
            return qMetaTypeId<CSettingKeyboardHotkey>();
        }

        bool CSettingKeyboardHotkey::isA(int metaTypeId) const
        {
            return (metaTypeId == qMetaTypeId<CSettingKeyboardHotkey>());
        }

        /*
         * Compare
         */
        int CSettingKeyboardHotkey::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CSettingKeyboardHotkey &>(otherBase);
            return compare(TupleConverter<CSettingKeyboardHotkey>::toMetaTuple(*this), TupleConverter<CSettingKeyboardHotkey>::toMetaTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CSettingKeyboardHotkey::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CSettingKeyboardHotkey>::toMetaTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CSettingKeyboardHotkey::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CSettingKeyboardHotkey>::toMetaTuple(*this);
        }

        /*
         * Hash
         */
        uint CSettingKeyboardHotkey::getValueHash() const
        {
            return qHash(TupleConverter<CSettingKeyboardHotkey>::toMetaTuple(*this));
        }

        /*
         * Equal?
         */
        bool CSettingKeyboardHotkey::operator ==(const CSettingKeyboardHotkey &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CSettingKeyboardHotkey>::toMetaTuple(*this) == TupleConverter<CSettingKeyboardHotkey>::toMetaTuple(other);
        }

        /*
         * Unequal?
         */
        bool CSettingKeyboardHotkey::operator !=(const CSettingKeyboardHotkey &other) const
        {
            return !((*this) == other);
        }

        bool CSettingKeyboardHotkey::operator< (CSettingKeyboardHotkey const &other) const
        {
            if (this->getKey() < other.getKey())
                return true;
            else
                return false;
        }

        void CSettingKeyboardHotkey::setKey(const Hardware::CKeyboardKey &key)
        {
            m_key = key;
        }

        void CSettingKeyboardHotkey::setObject(const CSettingKeyboardHotkey &obj)
        {
            m_hotkeyFunction = obj.m_hotkeyFunction;
            m_key = obj.m_key;
        }

        QVariant CSettingKeyboardHotkey::propertyByIndex(int index) const
        {
            switch (index)
            {
            case IndexFunction:
                return QVariant(m_hotkeyFunction.getFunction());
            case IndexFunctionAsString:
                return QVariant(m_hotkeyFunction.getFunctionAsString());
            case IndexModifier1:
                return QVariant(static_cast<uint>(m_key.getModifier1()));
            case IndexModifier2:
                return QVariant(static_cast<uint>(m_key.getModifier2()));
            case IndexModifier1AsString:
                return QVariant(m_key.getModifier1AsString());
            case IndexModifier2AsString:
                return QVariant(m_key.getModifier2AsString());
            case IndexKey:
                return QVariant(m_key.getKeyAsQtKey());
            case IndexKeyAsString:
                return QVariant(QString(QChar(m_key.getKeyAsQtKey())));
            case IndexKeyAsStringRepresentation:
                return QVariant(m_key.getKeyAsStringRepresentation());
            default:
                break;
            }

            Q_ASSERT_X(false, "CSettingKeyboardHotkey", "index unknown");
            QString m = QString("no property, index ").append(QString::number(index));
            return QVariant::fromValue(m);
        }

        void CSettingKeyboardHotkey::setPropertyByIndex(const QVariant &variant, int index)
        {
            switch (index)
            {
            case IndexFunction:
                {
                    uint f = variant.value<uint>();
                    m_hotkeyFunction.setFunction(static_cast<CHotkeyFunction::Function>(f));
                    break;
                }
            case IndexKey:
            case IndexKeyAsString:
            case IndexKeyAsStringRepresentation:
                {
                    // static_cast see docu of variant.type()
                    if (static_cast<QMetaType::Type>(variant.type()) == QMetaType::QChar)
                        m_key.setKey(variant.value<QChar>());
                    else
                        m_key.setKey(variant.value<QString>());
                    break;
                }
            case IndexModifier1AsString:
                m_key.setModifier1(variant.value<QString>());
                break;
            case IndexModifier2AsString:
                m_key.setModifier2(variant.value<QString>());
                break;
            case IndexObject:
                this->setObject(variant.value<BlackMisc::Settings::CSettingKeyboardHotkey>());
                break;
            default:
                Q_ASSERT_X(false, "CSettingKeyboardHotkey", "index unknown (setter)");
                break;
            }
        }
    } // namespace Hardware
} // BlackMisc
