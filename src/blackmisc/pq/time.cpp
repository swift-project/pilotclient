/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/pq/time.h"
#include "blackmisc/math/mathutils.h"
#include <QTime>
#include <QtGlobal>
#include <cmath>

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        CTime::CTime(int hours, int minutes, int seconds) : CPhysicalQuantity(0, CTimeUnit::nullUnit())
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

        CTime::CTime(QTime time, bool negative) : CPhysicalQuantity(0, CTimeUnit::nullUnit())
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
                const int hour = ts.midRef(0, 2).toInt();
                const int minute = ts.midRef(3, 2).toInt();
                int second = 0;
                if (ts.length() == 8) second = ts.midRef(6, 2).toInt();
                (*this) = CTime(hour, minute, second);

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

        QList<int> CTime::getHrsMinSecParts() const
        {
            using BlackMisc::Math::CMathUtils;
            CTime copy(*this);

            copy.switchUnit(CTimeUnit::s());

            using BlackMisc::Math::CMathUtils;
            double currentValue = copy.value();
            double hr = CMathUtils::trunc(currentValue / 3600);
            double remaining = std::fmod(currentValue, 3600);
            double mi = CMathUtils::trunc(remaining / 60);
            double se = CMathUtils::trunc(std::fmod(remaining, 60));

            QList<int> parts;
            parts << hr << mi << se;
            return parts;
        }

        QString CTime::formattedHrsMinSec() const
        {
            QList<int> parts = getHrsMinSecParts();
            QString h = QString("00%1").arg(QString::number(parts.at(0))).right(2);
            QString m = QString("00%1").arg(QString::number(parts.at(1))).right(2);
            QString s = QString("00%1").arg(QString::number(parts.at(2))).right(2);

            QString fs = QString("%1:%2:%3").arg(h, m, s);
            if (this->isNegativeWithEpsilonConsidered())
            {
                return QString("-").append(fs);
            }
            else
            {
                return fs;
            }
        }

        int CTime::toMs() const
        {
            double ms = this->valueRounded(CTimeUnit::ms(), 0);
            return static_cast<int>(ms);
        }

        int CTime::toSeconds() const
        {
            double ms = this->valueRounded(CTimeUnit::s(), 0);
            return static_cast<int>(ms);
        }

        QString CTime::formattedHrsMin() const
        {
            QList<int> parts = getHrsMinSecParts();
            QString h = QString("00%1").arg(QString::number(parts.at(0))).right(2);
            QString m = QString("00%1").arg(QString::number(parts.at(1))).right(2);
            QString fs = QString("%1:%2").arg(h, m);
            if (this->isNegativeWithEpsilonConsidered())
            {
                return QString("-").append(fs);
            }
            else
            {
                return fs;
            }
        }
    } // ns
} // ns
