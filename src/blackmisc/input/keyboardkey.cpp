/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/input/keyboardkey.h"

#include <QChar>
#include <QHash>
#include <QtDebug>
#include <QtGlobal>

#include "blackmisc/propertyindexref.h"

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Input, CKeyboardKey)

namespace BlackMisc::Input
{
    CKeyboardKey::CKeyboardKey() :
        m_keyCode(Key_Unknown)
    {}

    CKeyboardKey::CKeyboardKey(KeyCode keyCode) :
        m_keyCode(keyCode)
    {}

    QString CKeyboardKey::convertToQString(bool /* i18n */) const
    {
        return this->getKeyAsString();
    }

    void CKeyboardKey::setKeyObject(CKeyboardKey key)
    {
        m_keyCode = key.m_keyCode;
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
        case Key_Multiply: return QStringLiteral("NumMultiply"); // return QStringLiteral(u"×");
        case Key_Divide:   return QStringLiteral("NumDivide");   // return QStringLiteral(u"÷");
        case Key_Numpad0:  return QStringLiteral("Num0");
        case Key_Numpad1:  return QStringLiteral("Num1");
        case Key_Numpad2:  return QStringLiteral("Num2");
        case Key_Numpad3:  return QStringLiteral("Num3");
        case Key_Numpad4:  return QStringLiteral("Num4");
        case Key_Numpad5:  return QStringLiteral("Num5");
        case Key_Numpad6:  return QStringLiteral("Num6");
        case Key_Numpad7:  return QStringLiteral("Num7");
        case Key_Numpad8:  return QStringLiteral("Num8");
        case Key_Numpad9:  return QStringLiteral("Num9");
        case Key_NumpadEqual: return QStringLiteral("Num=");
        case Key_NumpadMinus: return QStringLiteral("Num-");
        case Key_NumpadPlus: return QStringLiteral("Num+");
        case Key_NumpadDelete: return QStringLiteral("Num.");
        case Key_Esc:      return QStringLiteral("Esc");
        case Key_Space:    return QStringLiteral("Space");
        case Key_Tab:      return QStringLiteral("Tab");
        case Key_Back:     return QStringLiteral("Bksp");
        case Key_Insert:   return QStringLiteral("Ins");
        case Key_Delete:   return QStringLiteral("Del");
        case Key_Home:     return QStringLiteral("Home");
        case Key_End:      return QStringLiteral("End");
        case Key_PageUp:   return QStringLiteral("PgUp");
        case Key_PageDown: return QStringLiteral("PgDn");
        case Key_CapsLock: return QStringLiteral("CapsLock");
        case Key_OEM1:     return QStringLiteral("OEM1");
        case Key_OEM2:     return QStringLiteral("OEM2");
        case Key_OEM3:     return QStringLiteral("OEM3");
        case Key_OEM4:     return QStringLiteral("OEM4");
        case Key_OEM5:     return QStringLiteral("OEM5");
        case Key_OEM6:     return QStringLiteral("OEM6");
        case Key_OEM7:     return QStringLiteral("OEM7");
        case Key_OEM8:     return QStringLiteral("OEM8");
        case Key_OEM102:   return QStringLiteral("OEM102");
        case Key_DeadGrave: return QStringLiteral("DeadGrave");
        case Key_Function1: return QStringLiteral("F1");
        case Key_Function2: return QStringLiteral("F2");
        case Key_Function3: return QStringLiteral("F3");
        case Key_Function4: return QStringLiteral("F4");
        case Key_Function5: return QStringLiteral("F5");
        case Key_Function6: return QStringLiteral("F6");
        case Key_Function7: return QStringLiteral("F7");
        case Key_Function8: return QStringLiteral("F8");
        case Key_Function9: return QStringLiteral("F9");
        case Key_Function10: return QStringLiteral("F10");
        case Key_Function11: return QStringLiteral("F11");
        case Key_Function12: return QStringLiteral("F12");
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

    QVariant CKeyboardKey::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexKey: return QVariant::fromValue(m_keyCode);
        case IndexKeyAsString: return QVariant::fromValue(getKeyAsString());
        default: break;
        }

        Q_ASSERT_X(false, "CKeyboardKey", "index unknown");
        QString m = QString("no property, index ").append(index.toQString());
        return QVariant::fromValue(m);
    }

    void CKeyboardKey::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CKeyboardKey>(); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
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
} // ns
