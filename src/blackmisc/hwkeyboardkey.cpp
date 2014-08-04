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
            m_qtKey(Qt::Key_unknown), m_modifier1(ModifierNone), m_modifier2(ModifierNone)
        {}

        CKeyboardKey::CKeyboardKey(Qt::Key keyCode, Modifier modifier1, Modifier modifier2) :
            m_qtKey(keyCode), m_modifier1(modifier1), m_modifier2(modifier2)
        {}

        void CKeyboardKey::registerMetadata()
        {
            qRegisterMetaType<CKeyboardKey>();
            qDBusRegisterMetaType<CKeyboardKey>();
        }

        /*
         * To JSON
         */
        QJsonObject CKeyboardKey::toJson() const
        {
            return BlackMisc::serializeJson(TupleConverter<CKeyboardKey>::toMetaTuple(*this));
        }

        /*
         * From Json
         */
        void CKeyboardKey::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, TupleConverter<CKeyboardKey>::toMetaTuple(*this));
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
            return compare(TupleConverter<CKeyboardKey>::toMetaTuple(*this), TupleConverter<CKeyboardKey>::toMetaTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CKeyboardKey::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CKeyboardKey>::toMetaTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CKeyboardKey::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CKeyboardKey>::toMetaTuple(*this);
        }

        /*
         * Hash
         */
        uint CKeyboardKey::getValueHash() const
        {
            return qHash(TupleConverter<CKeyboardKey>::toMetaTuple(*this));
        }

        /*
         * Equal?
         */
        bool CKeyboardKey::operator ==(const CKeyboardKey &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CKeyboardKey>::toMetaTuple(*this) == TupleConverter<CKeyboardKey>::toMetaTuple(other);
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

        bool CKeyboardKey::addModifier(const Modifier &modifier)
        {
            bool added = false;

            // Don't add the same modifier twice
            if (hasModifier(modifier))
                return false;

            if (m_modifier1 == ModifierNone)
            {
                m_modifier1 = modifier;
                added = true;
            }
            else if (m_modifier2 == ModifierNone)
            {
                m_modifier2 = modifier;
                added = true;
            }

            if (added) this->cleanup();
            return added;
        }

        bool CKeyboardKey::addModifier(const QString &modifier)
        {
            return addModifier(modifierFromString(modifier));
        }

        bool CKeyboardKey::removeModifier(const Modifier &modifier)
        {
            bool removed = false;

            if (m_modifier1 == modifier)
            {
                m_modifier1 = ModifierNone;
                removed = true;
            }
            if (m_modifier2 == modifier)
            {
                m_modifier2 = ModifierNone;
                removed = true;
            }
            cleanup();
            return removed;
        }

        bool CKeyboardKey::removeModifier(const QString &modifier)
        {
            return removeModifier(modifierFromString(modifier));
        }

        bool CKeyboardKey::equalsModifierReleaxed(CKeyboardKey::Modifier m1, CKeyboardKey::Modifier m2)
        {
            if (m1 == m2) return true;
            if (m1 == ModifierAltAny && (m2 == ModifierAltLeft || m2 == ModifierAltRight)) return true;
            if (m1 == ModifierCtrlAny && (m2 == ModifierCtrlLeft || m2 == ModifierCtrlRight)) return true;
            if (m1 == ModifierShiftAny && (m2 == ModifierShiftLeft || m2 == ModifierShiftRight)) return true;

            if (m2 == ModifierAltAny && (m1 == ModifierAltLeft || m1 == ModifierAltRight)) return true;
            if (m2 == ModifierCtrlAny && (m1 == ModifierCtrlLeft || m1 == ModifierCtrlRight)) return true;
            if (m2 == ModifierShiftAny && (m1 == ModifierShiftLeft || m1 == ModifierShiftRight)) return true;

            return false;
        }

        void CKeyboardKey::setKeyObject(const CKeyboardKey &key)
        {
            this->m_modifier1 = key.m_modifier1;
            this->m_modifier2 = key.m_modifier2;
            this->m_qtKey = key.m_qtKey;
        }

        bool CKeyboardKey::equalsWithRelaxedModifiers(const CKeyboardKey &key) const
        {
            if (key == (*this)) return true; // fully equal, not need to bother

            // this can never be true
            if (key.m_qtKey != this->m_qtKey) return false;
            if (this->numberOfModifiers() != key.numberOfModifiers()) return false;

            // special modifiers
            if (this->getModifier1() != key.getModifier1())
            {
                if (!CKeyboardKey::equalsModifierReleaxed(this->getModifier1(), key.getModifier1())) return false;
            }
            return CKeyboardKey::equalsModifierReleaxed(this->getModifier1(), key.getModifier1());
        }

        QVariant CKeyboardKey::propertyByIndex(int index) const
        {
            switch (index)
            {
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
