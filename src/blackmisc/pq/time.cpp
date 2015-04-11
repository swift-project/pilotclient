/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/pq/time.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        CTime::CTime(int hours, int minutes, int seconds) : CValueObject(0, CTimeUnit::nullUnit())
        {
            bool negative = (hours < 0);
            double value = qAbs(hours) + minutes / 100.0 + seconds / 10000.0;
            if (minutes == 0 && seconds == 0)
            {
                (*this) = CTime(hours, CTimeUnit::h());
            }
            else
            {
                if (seconds == 0)
                    (*this) = CTime(value, CTimeUnit::hrmin());
                else
                    (*this) = CTime(value, CTimeUnit::hms());
            }
            if (negative)
            {
                this->makeNegative();
            }
        }

        CTime::CTime(const QTime &time, bool negative) : CValueObject(0, CTimeUnit::nullUnit())
        {
            CTime converted(time.hour(), time.minute(), time.second());
            (*this) = converted;
            if (negative)
            {
                this->makeNegative();
            }
        }

        void CTime::parseFromString(const QString &time)
        {
            QTime t;
            QString ts = time.trimmed();

            // deal with sign
            double factor = 1.0;
            if (ts.startsWith('+'))
            {
                ts.remove(0, 1);
            }
            else if (ts.startsWith('-'))
            {
                factor = -1.0;
                ts.remove(0, 1);
            }

            if (ts.contains(":") && (ts.length() == 8 || ts.length() == 5))
            {
                if (ts.length() == 5)
                    t = QTime::fromString(ts, "hh:mm");
                else if (ts.length() == 8)
                    t = QTime::fromString(ts, "hh:mm:ss");
                (*this) = CTime(t);

                // fix sign if required
                if (factor < 0)
                {
                    this->setValueSameUnit(this->value() * factor);
                }
            }
            else
                CPhysicalQuantity::parseFromString(time);
        }

        void CTime::parseFromString(const QString &time, CPqString::SeparatorMode mode)
        {
            this->parseFromString(time);
            Q_UNUSED(mode);
        }

        QTime CTime::toQTime() const
        {
            CTime copy(*this);
            copy.setUnit(CTimeUnit::hms());

            // QTime is not defined for negative numbers
            // so we use the absolute value here
            copy.makePositive();

            // convert
            QString s = copy.toQString(false).replace('h', ':').replace('m', ':').replace('s', "");
            QTime t = s.length() == 8 ?
                      QTime::fromString(s, "hh:mm:ss") :
                      QTime::fromString(s, "hh:mm");
            return t;
        }

        QList<int> CTime::getHrsMinSecParts() const
        {
            QTime t = this->toQTime();
            QList<int> parts;
            parts << t.hour() << t.minute() << t.second();
            return parts;
        }

        QString CTime::formattedHrsMinSec() const
        {
            QList<int> parts = getHrsMinSecParts();
            QString h = QString("00%1").arg(QString::number(parts.at(0))).right(2);
            QString m = QString("00%1").arg(QString::number(parts.at(1))).right(2);
            QString s = QString("00%1").arg(QString::number(parts.at(2))).right(2);

            QString fs = QString("%1:%2:%3").arg(h).arg(m).arg(s);
            if (this->isNegativeWithEpsilonConsidered())
            {
                return QString("-").append(fs);
            }
            else
            {
                return fs;
            }
        }

        QString CTime::formattedHrsMin() const
        {
            QList<int> parts = getHrsMinSecParts();
            QString h = QString("00%1").arg(QString::number(parts.at(0))).right(2);
            QString m = QString("00%1").arg(QString::number(parts.at(1))).right(2);
            QString fs = QString("%1:%2").arg(h).arg(m);
            if (this->isNegativeWithEpsilonConsidered())
            {
                return QString("-").append(fs);
            }
            else
            {
                return fs;
            }
        }
    }
}
