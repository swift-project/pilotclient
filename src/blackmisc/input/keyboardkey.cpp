/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/input/keyboardkey.h"

#include <QChar>
#include <QHash>
#include <QtDebug>
#include <QtGlobal>

#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

namespace BlackMisc
{
    namespace Input
    {
        CKeyboardKey::CKeyboardKey() :
            m_keyCode(Key_Unknown)
        {}

        CKeyboardKey::CKeyboardKey(KeyCode keyCode) :
            m_keyCode(keyCode)
        {}

        QString CKeyboardKey::convertToQString(bool /* i18n */) const
        {
            return getKeyAsString();
        }

        void CKeyboardKey::setKeyObject(CKeyboardKey key)
        {
            this->m_keyCode = key.m_keyCode;
        }

        QString CKeyboardKey::getKeyAsString() const
        {
            if (m_keyCode == Key_Unknown) return QString();

            static const QHash<KeyCode, QString> keyStrings =
            {
                { Key_ShiftLeft, QStringLiteral("ShiftLeft") },
                { Key_ShiftRight, QStringLiteral("ShiftRight") },
                { Key_ControlLeft, QStringLiteral("CtrlLeft") },
                { Key_ControlRight, QStringLiteral("CtrlRight") },
                { Key_AltLeft, QStringLiteral("AltLeft") },
                { Key_AltRight, QStringLiteral("AltRight") }
            };

            if (isModifier()) { return keyStrings.value(m_keyCode); }

            switch (m_keyCode)
            {
            case Key_Multiply: return QStringLiteral("×");
            case Key_Divide: return QStringLiteral("÷");
            case Key_Numpad0: return QStringLiteral("Num0");
            case Key_Numpad1: return QStringLiteral("Num1");
            case Key_Numpad2: return QStringLiteral("Num2");
            case Key_Numpad3: return QStringLiteral("Num3");
            case Key_Numpad4: return QStringLiteral("Num4");
            case Key_Numpad5: return QStringLiteral("Num5");
            case Key_Numpad6: return QStringLiteral("Num6");
            case Key_Numpad7: return QStringLiteral("Num7");
            case Key_Numpad8: return QStringLiteral("Num8");
            case Key_Numpad9: return QStringLiteral("Num9");
            default: return QChar::fromLatin1(static_cast<char>(m_keyCode));
            }
        }

        const QList<KeyCode> &CKeyboardKey::allModifiers()
        {
            static const QList<KeyCode> allModifiers =
            {
                Key_ShiftLeft,
                Key_ShiftRight,
                Key_ControlLeft,
                Key_ControlRight,
                Key_AltLeft,
                Key_AltRight,
            };
            return allModifiers;
        }

        CVariant CKeyboardKey::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexKey:
                return CVariant::from(this->m_keyCode);
            case IndexKeyAsString:
                return CVariant::from(getKeyAsString());
            default:
                break;
            }

            Q_ASSERT_X(false, "CKeyboardKey", "index unknown");
            QString m = QString("no property, index ").append(index.toQString());
            return CVariant::fromValue(m);
        }

        void CKeyboardKey::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CKeyboardKey>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexKey:
            case IndexKeyAsString:
                qFatal("Not implemented");
                break;
            case IndexKeyObject:
                this->setKeyObject(variant.value<CKeyboardKey>());
                break;
            default:
                Q_ASSERT_X(false, "CKeyboardKey", "index unknown (setter)");
                break;
            }
        }
    } // namespace Hardware
} // BlackMisc
