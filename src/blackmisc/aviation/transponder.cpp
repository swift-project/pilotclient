/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/transponder.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/variant.h"

#include <QRegularExpression>
#include <Qt>
#include <QtDebug>

namespace BlackMisc
{
    namespace Aviation
    {
        void CTransponder::registerMetadata()
        {
            CValueObject<CTransponder>::registerMetadata();
            qRegisterMetaType<TransponderMode>();
            qDBusRegisterMetaType<CTransponder::TransponderMode>();
        }

        CTransponder::CTransponder(int transponderCode, CTransponder::TransponderMode transponderMode) :
            m_transponderCode(transponderCode), m_transponderMode(transponderMode)
        {  }

        CTransponder::CTransponder(int transponderCode, const QString &transponderMode) :
            m_transponderCode(transponderCode), m_transponderMode(StateStandby)
        {
            this->setModeAsString(transponderMode);
        }

        CTransponder::CTransponder(const QString &transponderCode, CTransponder::TransponderMode transponderMode) :
            m_transponderCode(0), m_transponderMode(transponderMode)
        {
            bool ok = false;
            m_transponderCode = transponderCode.toInt(&ok);
            if (!ok) m_transponderCode = -1; // will cause assert / exception
        }

        CTransponder::CTransponder(const QString &transponderCode, const QString &transponderMode) :
            m_transponderCode(0), m_transponderMode(StateStandby)
        {
            bool ok = false;
            m_transponderCode = transponderCode.toInt(&ok);
            if (!ok) m_transponderCode = -1; // will cause assert / exception
            this->setModeAsString(transponderMode);
        }

        bool CTransponder::validValues() const
        {
            return CTransponder::isValidTransponderCode(m_transponderCode);
        }

        bool CTransponder::isInNormalSendingMode() const
        {
            switch (this->getTransponderMode())
            {
            case ModeA:
            case ModeC:
            case ModeMil1:
            case ModeMil2:
            case ModeMil3:
            case ModeMil4:
            case ModeMil5:
                return true;
            case StateIdent:
            case StateStandby:
            default:
                return false;
            }
        }

        void CTransponder::toggleTransponderMode()
        {
            if (this->getTransponderMode() == StateIdent || this->isInNormalSendingMode())
            {
                this->setTransponderMode(StateStandby);
            }
            else
            {
                this->setTransponderMode(ModeC);
            }
        }

        QString CTransponder::convertToQString(bool /* i18n */) const
        {
            return this->getTransponderCodeFormatted().append(" ").append(this->getModeAsString());
        }

        CTransponder::TransponderMode CTransponder::modeFromString(const QString &modeString)
        {
            if (modeString.startsWith("I", Qt::CaseInsensitive)) return StateIdent;
            if (modeString.startsWith("S", Qt::CaseInsensitive)) return StateStandby;
            if (modeString.startsWith("Mode C", Qt::CaseInsensitive)) return ModeC;
            if (modeString.startsWith("C", Qt::CaseInsensitive)) return ModeC;
            if (modeString.startsWith("Mode S", Qt::CaseInsensitive)) return ModeS;
            if (modeString.contains("1", Qt::CaseInsensitive)) return ModeMil1;
            if (modeString.contains("2", Qt::CaseInsensitive)) return ModeMil2;
            if (modeString.contains("3", Qt::CaseInsensitive)) return ModeMil3;
            if (modeString.contains("4", Qt::CaseInsensitive)) return ModeMil4;
            if (modeString.contains("5", Qt::CaseInsensitive)) return ModeMil5;
            return StateStandby;
        }

        bool CTransponder::setTransponderMode(CTransponder::TransponderMode mode)
        {
            const int m = static_cast<int>(mode);
            if (m == m_transponderMode) { return false; }
            m_transponderMode = m;
            return true;
        }

        QString CTransponder::getTransponderCodeFormatted() const
        {
            QString f("0000");
            f = f.append(QString::number(m_transponderCode));
            return f.right(4);
        }

        QString CTransponder::getTransponderCodeAndModeFormatted() const
        {
            QString s = this->getTransponderCodeFormatted();
            s.append(' ').append(this->getModeAsString());
            return s;
        }

        void CTransponder::setTransponderCode(const QString &transponderCode)
        {
            if (CTransponder::isValidTransponderCode(transponderCode))
            {
                bool ok;
                this->setTransponderCode(transponderCode.toInt(&ok));
            }
            else
            {
                Q_ASSERT_X(false, "CTransponder::setTransponderCode", "illegal transponder value");
            }
        }

        bool CTransponder::isValidTransponderCode(const QString &transponderCode)
        {
            if (transponderCode.isEmpty() || transponderCode.length() > 4) return false;
            bool number;
            int tc = transponderCode.toInt(&number);
            if (!number) return false;
            if (tc < 0 || tc > 7777) return false;
            thread_local const QRegularExpression rx("^[0-7]{1,4}$");
            return rx.match(transponderCode).hasMatch();
        }

        bool CTransponder::isValidTransponderCode(qint32 transponderCode)
        {
            if (transponderCode < 0 || transponderCode > 7777) return false;
            return CTransponder::isValidTransponderCode(QString::number(transponderCode));
        }

        CTransponder CTransponder::getStandardTransponder(qint32 transponderCode, CTransponder::TransponderMode mode)
        {
            return CTransponder(transponderCode, mode);
        }

        const QString &CTransponder::modeAsString(CTransponder::TransponderMode mode)
        {
            static const QString i("Ident");
            static const QString s("Standby");
            static const QString mc("Mode C");
            static const QString ms("Mode S");
            static const QString m1("Mil.Mode 1");
            static const QString m2("Mil.Mode 2");
            static const QString m3("Mil.Mode 3");
            static const QString m4("Mil.Mode 4");
            static const QString m5("Mil.Mode 5");

            switch (mode)
            {
            case StateIdent:   return i;
            case StateStandby: return s;
            case ModeC:    return mc;
            case ModeS:    return ms;
            case ModeMil1: return m1;
            case ModeMil2: return m2;
            case ModeMil3: return m3;
            case ModeMil4: return m4;
            case ModeMil5: return m5;
            default: qFatal("Illegal Transponder Mode");
            }
            return i;
        }

        const QString &CTransponder::modeAsShortString(CTransponder::TransponderMode mode)
        {
            static const QString i("I");
            static const QString s("S");
            static const QString mc("C");
            static const QString ms("S");
            static const QString m1("1");
            static const QString m2("2");
            static const QString m3("3");
            static const QString m4("4");
            static const QString m5("5");

            switch (mode)
            {
            case StateIdent:   return i;
            case StateStandby: return s;
            case ModeC:    return mc;
            case ModeS:    return ms;
            case ModeMil1: return m1;
            case ModeMil2: return m2;
            case ModeMil3: return m3;
            case ModeMil4: return m4;
            case ModeMil5: return m5;
            default: qFatal("Illegal Transponder Mode");
            }
            return i;
        }

        QVariant CTransponder::propertyByIndex(CPropertyIndexRef index) const
        {
            if (index.isMyself()) { return QVariant::fromValue(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexMode: return QVariant::fromValue(this->getTransponderMode());
            case IndexModeAsString: return QVariant::fromValue(this->getModeAsString());
            case IndexTransponderCode: return QVariant::fromValue(this->getTransponderCode());
            case IndexTransponderCodeFormatted: return QVariant::fromValue(this->getTransponderCodeFormatted());
            case IndexTransponderCodeAndModeFormatted: return QVariant::fromValue(this->getTransponderCodeAndModeFormatted());
            default: break;
            }

            Q_ASSERT_X(false, "CTransponder", "index unknown");
            const QString m = QString("no property, index ").append(index.toQString());
            return QVariant::fromValue(m);
        }

        void CTransponder::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.value<CTransponder>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexMode: m_transponderMode = variant.toInt(); break;
            case IndexModeAsString: this->setTransponderMode(modeFromString(variant.toString())); break;
            case IndexTransponderCode:
            case IndexTransponderCodeFormatted:
                if (variant.canConvert<int>())
                {
                    this->setTransponderCode(variant.toInt());
                }
                else
                {
                    this->setTransponderCode(variant.toString());
                }
                break;
            case IndexTransponderCodeAndModeFormatted:
            default:
                Q_ASSERT_X(false, "CTransponder", "index unknown");
                break;
            }
        }
    } // namespace
} // namespace
