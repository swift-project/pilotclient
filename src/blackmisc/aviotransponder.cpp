/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/aviotransponder.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {

        /*
         * Valid values?
         */
        bool CTransponder::validValues() const
        {
            if (this->isDefaultValue()) return true; // special case
            return CTransponder::isValidTransponderCode(this->m_transponderCode);
        }

        /*
         * Validate
         */
        bool CTransponder::validate(bool strict) const
        {
            if (this->isDefaultValue()) return true;
            bool valid = this->validValues();
            if (!strict) return valid;
            Q_ASSERT_X(valid, "CTransponder::validate", "illegal values");
            if (!valid) throw std::range_error("Illegal values in CTransponder::validate");
            return true;
        }

        /*
         * String representation
         */
        QString CTransponder::convertToQString(bool /* i18n */) const
        {
            QString s = this->getName();
            s = s.append(" ").append(this->getTransponderCodeFormatted()).append(" ").append(this->getModeAsString());
            return s;
        }

        /*
         * Mode as readable string
         */
        QString CTransponder::getModeAsString() const
        {
            QString m;
            switch (this->getTransponderMode())
            {
            case StateIdent:
                m = "Ident";
                break;
            case StateStandby:
                m = "Standby";
                break;
            case ModeC:
                m = "Mode C";
                break;
            case ModeS:
                m = "Mode S";
                break;
            case ModeMil1:
                m = "Mil.Mode 1";
                break;
            case ModeMil2:
                m = "Mil.Mode 2";
                break;
            case ModeMil3:
                m = "Mil.Mode 3";
                break;
            case ModeMil4:
                m = "Mil.Mode 4";
                break;
            case ModeMil5:
                m = "Mil.Mode 5";
                break;
            default:
                throw std::range_error("Illegal Transponder Mode");
            }
            return m;
        }

        /*
         * Mode as readable string
         */
        void CTransponder::setModeAsString(const QString &m)
        {
            if (m == "Ident")
            {
                this->setTransponderMode(StateIdent);
            }
            else if (m == "Standby")
            {
                this->setTransponderMode(StateStandby);
            }
            else if (m == "Mode C")
            {
                this->setTransponderMode(ModeC);
            }
            else if (m == "Mode S")
            {
                this->setTransponderMode(ModeS);
            }
            else if (m == "Mil.Mode 1")
            {
                this->setTransponderMode(ModeMil1);
            }
            else if (m == "Mil.Mode 2")
            {
                this->setTransponderMode(ModeMil2);
            }
            else if (m == "Mil.Mode 3")
            {
                this->setTransponderMode(ModeMil3);
            }
            else if (m == "Mil.Mode 4")
            {
                this->setTransponderMode(ModeMil4);
            }
            else if (m == "Mil.Mode 5")
            {
                this->setTransponderMode(ModeMil5);
            }
            else
            {
                throw std::range_error("Illegal Transponder Mode");
            }
        }

        /*
         * Formatted transponder code
         */
        QString CTransponder::getTransponderCodeFormatted() const
        {
            QString f("0000");
            f = f.append(QString::number(this->m_transponderCode));
            return f.right(4);
        }

        /*
         * Formatted transponder code + mode
         */
        QString CTransponder::getTransponderCodeAndModeFormatted() const
        {
            QString s = this->getTransponderCodeFormatted();
            s.append(' ').append(this->getModeAsString());
            return s;
        }

        /*
         * Transponder by string
         */
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

        /*
         * Valid code?
         */
        bool CTransponder::isValidTransponderCode(const QString &transponderCode)
        {
            if (transponderCode.isEmpty() || transponderCode.length() > 4) return false;
            bool number;
            qint32 tc = transponderCode.toInt(&number);
            if (!number) return false;
            if (tc < 0 || tc > 7777) return false;
            QRegExp rx("[0-7]{1,4}");
            return rx.exactMatch(transponderCode);
        }

        /*
         * Valid code?
         */
        bool CTransponder::isValidTransponderCode(qint32 transponderCode)
        {
            if (transponderCode < 0 || transponderCode > 7777) return false;
            return CTransponder::isValidTransponderCode(QString::number(transponderCode));
        }

        /*
         * Stream to DBus <<
         */
        void CTransponder::marshallToDbus(QDBusArgument &argument) const
        {
            this->CAvionicsBase::marshallToDbus(argument);
            argument << this->m_transponderCode;
            argument << static_cast<qint32>(this->m_transponderMode);
        }

        /*!
         * Stream from DBus >>
         */
        void CTransponder::unmarshallFromDbus(const QDBusArgument &argument)
        {
            this->CAvionicsBase::unmarshallFromDbus(argument);
            qint32 tm;
            argument >> this->m_transponderCode;
            argument >> tm;
            this->m_transponderMode = static_cast<TransponderMode>(tm);
        }

        /*
         * Value hash
         */
        uint CTransponder::getValueHash() const
        {
            QList<uint> hashs;
            hashs << qHash(this->m_name);
            hashs << qHash(this->m_transponderCode);
            hashs << qHash(this->m_transponderMode);
            return BlackMisc::calculateHash(hashs, "CTransponder");
        }

        /*
         * Register metadata of unit and quantity
         */
        void CTransponder::registerMetadata()
        {
            qRegisterMetaType<CTransponder>();
            qDBusRegisterMetaType<CTransponder>();
        }

    } // namespace
} // namespace
