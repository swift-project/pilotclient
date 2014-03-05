/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hwkeyboardkey.h"
#include "blackmiscfreefunctions.h"
#include <QCoreApplication>

namespace BlackMisc
{
    namespace Hardware
    {
        CKeyboardKey::CKeyboardKey() :
            m_qtKey(Qt::Key_unknown), m_nativeVirtualKey(0), m_modifier1(ModifierNone), m_modifier2(ModifierNone), m_function(HotkeyNone)
        {}

        CKeyboardKey::CKeyboardKey(HotkeyFunction function) :
            m_qtKey(Qt::Key_unknown), m_nativeVirtualKey(0), m_modifier1(ModifierNone), m_modifier2(ModifierNone), m_function(function)
        {}


        CKeyboardKey::CKeyboardKey(Qt::Key keyCode, quint32 nativeVirtualKey, Modifier modifier1, Modifier modifier2, const HotkeyFunction &function) :
            m_qtKey(keyCode), m_nativeVirtualKey(nativeVirtualKey), m_modifier1(modifier1), m_modifier2(modifier2), m_function(function)
        {}

        void CKeyboardKey::registerMetadata()
        {
            qRegisterMetaType<CKeyboardKey>();
            qDBusRegisterMetaType<CKeyboardKey>();
        }

        QString CKeyboardKey::convertToQString(bool /* i18n */) const
        {
            QString s = this->getModifier1AsString();
            s.append(" ").append(this->getModifier2AsString()).append(" ");
            if (this->m_qtKey != 0) s.append(" ").append(this->getKeyAsStringRepresentation());
            return s.trimmed();
        }

        int CKeyboardKey::getMetaTypeId() const
        {
            return qMetaTypeId<CKeyboardKey>();
        }

        bool CKeyboardKey::isA(int metaTypeId) const
        {
            return (metaTypeId == qMetaTypeId<CKeyboardKey>());
        }

        QString CKeyboardKey::modifierToString(CKeyboardKey::Modifier modifier)
        {
            switch (modifier)
            {
            case ModifierNone: return "";
            case ModifierAltAny: return "Alt";
            case ModifierAltLeft: return "Alt Left";
            case ModifierAltRight: return "Alt Right";
            case ModifierShiftAny: return "Shift";
            case ModifierShiftLeft: return "Shift Left";
            case ModifierShiftRight: return "Shift Right";
            case ModifierCtrlAny: return "Ctrl";
            case ModifierCtrlLeft: return "Ctrl Left";
            case ModifierCtrlRight: return "Ctrl Right";
            case ModifierMeta: return "Meta";
            default:
                qFatal("Wrong modifier");
                return "";
            }
        }

        CKeyboardKey::Modifier CKeyboardKey::modifierFromString(const QString &modifier)
        {
            QString mod = modifier.toLower().replace('&', ' ');
            if (mod == "ctrl" || mod == "ctl") return ModifierCtrlAny;
            if (mod.startsWith("ctrl l") || mod.startsWith("ctl l")) return ModifierCtrlLeft;
            if (mod.startsWith("ctrl r") || mod.startsWith("ctl r")) return ModifierCtrlRight;

            if (mod == "shift") return ModifierShiftAny;
            if (mod.startsWith("shift l")) return ModifierShiftLeft;
            if (mod.startsWith("shift r")) return ModifierShiftRight;

            if (mod == "alt") return ModifierAltAny;
            if (mod.startsWith("alt l")) return ModifierAltLeft;
            if (mod.startsWith("alt r")) return ModifierAltRight;

            if (mod == "meta") return ModifierMeta;

            return ModifierNone;
        }

        CKeyboardKey::Modifier CKeyboardKey::modifierFromQtModifier(Qt::Modifier qtModifier)
        {
            switch (qtModifier)
            {
            case Qt::META : return ModifierMeta;
            case Qt::SHIFT : return ModifierShiftAny;
            case Qt::CTRL: return ModifierShiftAny;
            case Qt::ALT: return ModifierAltAny;
            default:
                return ModifierNone;
            }
        }

        const QStringList &CKeyboardKey::modifiers()
        {
            static QStringList modifiers;
            if (modifiers.isEmpty())
            {
                modifiers << modifierToString(ModifierNone);
                modifiers << modifierToString(ModifierCtrlAny);
                modifiers << modifierToString(ModifierCtrlLeft);
                modifiers << modifierToString(ModifierCtrlRight);
                modifiers << modifierToString(ModifierShiftAny);
                modifiers << modifierToString(ModifierShiftLeft);
                modifiers << modifierToString(ModifierShiftRight);
                modifiers << modifierToString(ModifierAltAny);
                modifiers << modifierToString(ModifierAltLeft);
                modifiers << modifierToString(ModifierAltRight);
                modifiers << modifierToString(ModifierMeta);
            }
            return modifiers;
        }

        /*
         * Compare
         */
        int CKeyboardKey::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CKeyboardKey &>(otherBase);
            return compare(TupleConverter<CKeyboardKey>::toTuple(*this), TupleConverter<CKeyboardKey>::toTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CKeyboardKey::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CKeyboardKey>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CKeyboardKey::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CKeyboardKey>::toTuple(*this);
        }

        /*
         * Hash
         */
        uint CKeyboardKey::getValueHash() const
        {
            return qHash(TupleConverter<CKeyboardKey>::toTuple(*this));
        }

        /*
         * Equal?
         */
        bool CKeyboardKey::operator ==(const CKeyboardKey &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CKeyboardKey>::toTuple(*this) == TupleConverter<CKeyboardKey>::toTuple(other);
        }

        /*
         * Unequal?
         */
        bool CKeyboardKey::operator !=(const CKeyboardKey &other) const
        {
            return !((*this) == other);
        }

        bool CKeyboardKey::operator< (CKeyboardKey const &other) const
        {
            if (this->getKey() < other.getKey())
                return true;
            else
                return false;
        }

        void CKeyboardKey::cleanup()
        {
            if (!this->hasModifier()) return;
            if (this->m_modifier1 == ModifierNone) qSwap(this->m_modifier1, this->m_modifier2);
            if (this->numberOfModifiers() == 1) return;
            if (this->m_modifier1 == this->m_modifier2)
            {
                // redundant
                this->m_modifier2 = ModifierNone;
                return;
            }

            // order
            if (static_cast<uint>(this->m_modifier1) < static_cast<uint>(this->m_modifier2))
                qSwap(this->m_modifier1, this->m_modifier2);
        }

        int CKeyboardKey::numberOfModifiers() const
        {
            if (!this->hasModifier()) return 0;
            if (this->m_modifier1 == ModifierNone || this->m_modifier2 == ModifierNone) return 1;
            return 2;
        }

        void CKeyboardKey::setKey(const QString &key)
        {
            if (key.isEmpty())
                this->m_qtKey = Qt::Key_unknown;

            QKeySequence sequence(key);
            m_qtKey = static_cast<Qt::Key>(sequence[0]);
        }

        void CKeyboardKey::setKey(const QChar &key)
        {
            if (key.isNull())
                this->m_qtKey = Qt::Key_unknown;
            else
            {
                QKeySequence sequence(key);
                m_qtKey = static_cast<Qt::Key>(sequence[0]);
            }
        }
        }

        QString CKeyboardKey::getFunctionAsString() const
        {
            switch (this->m_function)
            {
            case HotkeyNone: return "";
            case HotkeyPtt: return QCoreApplication::translate("Hotkey", "PTT");
            case HotkeyOpacity50: return QCoreApplication::translate("Hotkey", "Opacity 50%");
            case HotkeyOpacity100: return QCoreApplication::translate("Hotkey", "Opacity 100%");
            case HotkeyToggleCom1: return QCoreApplication::translate("Hotkey", "Toggle COM1");
            case HotkeyToggleCom2: return QCoreApplication::translate("Hotkey", "Toggle COM2");
            default:
                qFatal("Wrong function");
                return "";
            }

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("Hotkey", "PTT");
            (void)QT_TRANSLATE_NOOP("Hotkey", "Opacity 50%");
            (void)QT_TRANSLATE_NOOP("Hotkey", "Opacity 100%");
            (void)QT_TRANSLATE_NOOP("Hotkey", "Toggle COM1");
            (void)QT_TRANSLATE_NOOP("Hotkey", "Toggle COM2");
        }

        void CKeyboardKey::setKeyObject(const CKeyboardKey &key)
        {
            this->m_function = key.m_function;
            this->m_modifier1 = key.m_modifier1;
            this->m_modifier2 = key.m_modifier2;
            this->m_nativeVirtualKey = key.m_nativeVirtualKey;
            this->m_qtKey = key.m_qtKey;
        }

        QVariant CKeyboardKey::propertyByIndex(int index) const
        {
            switch (index)
            {
            case IndexFunction:
                return QVariant(this->m_function);
            case IndexFunctionAsString:
                return QVariant(this->getFunctionAsString());
            case IndexModifier1:
                return QVariant(static_cast<uint>(this->m_modifier1));
            case IndexModifier2:
                return QVariant(static_cast<uint>(this->m_modifier2));
            case IndexModifier1AsString:
                return QVariant(this->getModifier1AsString());
            case IndexModifier2AsString:
                return QVariant(this->getModifier2AsString());
            case IndexKey:
                return QVariant(this->m_qtKey);
            case IndexKeyAsString:
                return QVariant(QString(QChar(this->m_qtKey)));
            case IndexKeyAsStringRepresentation:
                return QVariant(this->getKeyAsStringRepresentation());
            default:
                break;
            }

            Q_ASSERT_X(false, "CKeyboardKey", "index unknown");
            QString m = QString("no property, index ").append(QString::number(index));
            return QVariant::fromValue(m);
        }

        QString CKeyboardKey::toStringRepresentation(int key)
        {
            if (key == 0) return "";
            QString ks = QKeySequence(key).toString();
            // ks.append('(').append(QString::number(key)).append(')');
            return ks;
        }

        void CKeyboardKey::setPropertyByIndex(const QVariant &variant, int index)
        {
            switch (index)
            {
            case IndexFunction:
                {
                    uint f = variant.value<uint>();
                    this->setFunction(static_cast<HotkeyFunction>(f));
                    break;
                }
            case IndexKey:
            case IndexKeyAsString:
            case IndexKeyAsStringRepresentation:
                {
                    // static_cast see docu of variant.type()
                    if (static_cast<QMetaType::Type>(variant.type()) == QMetaType::QChar)
                        this->setKey(variant.value<QChar>());
                    else
                        this->setKey(variant.value<QString>());
                    break;
                }
            case IndexModifier1AsString:
                this->setModifier1(variant.value<QString>());
                break;
            case IndexModifier2AsString:
                this->setModifier2(variant.value<QString>());
                break;
            case IndexKeyObject:
                this->setKeyObject(variant.value<BlackMisc::Hardware::CKeyboardKey>());
                break;
            default:
                Q_ASSERT_X(false, "CKeyboardKey", "index unknown (setter)");
                break;
            }
        }
    } // namespace Hardware
} // BlackMisc
