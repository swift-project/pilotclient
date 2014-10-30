/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
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

        QString CSettingKeyboardHotkey::convertToQString(bool /* i18n */) const
        {
            return m_key.toQString();
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

        QVariant CSettingKeyboardHotkey::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
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
                return CValueObject::propertyByIndex(index);
            }
        }

        void CSettingKeyboardHotkey::setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromQVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
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
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

    } // namespace Hardware
} // BlackMisc
