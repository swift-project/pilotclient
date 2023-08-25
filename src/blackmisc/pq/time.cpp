// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/pq/time.h"
#include "blackmisc/math/mathutils.h"
#include <QTime>
#include <QtGlobal>
#include <cmath>

using BlackMisc::Math::CMathUtils;

namespace BlackMisc::PhysicalQuantities
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
        {
            CPhysicalQuantity::parseFromString(time);
        }
    }

    bool CTime::parseFromString_hhmm(const QString &hhmm)
    {
        if (hhmm.length() != 4) { return false; }
        const QString hStr = hhmm.left(2);
        const QString mStr = hhmm.right(2);

        bool ok;
        const int h = hStr.toInt(&ok);
        if (!ok || h < 0 || h > 23) { return false; }

        const int m = mStr.toInt(&ok);
        if (!ok || m < 0 || m > 59) { return false; }

        *this = CTime(h, m, 0);
        return true;
    }

    void CTime::parseFromString(const QString &time, CPqString::SeparatorMode mode)
    {
        this->parseFromString(time);
        Q_UNUSED(mode);
    }

    QList<int> CTime::getHrsMinSecParts() const
    {
        CTime copy(*this);
        copy.switchUnit(CTimeUnit::s());

        const double currentValue = copy.value();
        const double hr = CMathUtils::trunc(currentValue / 3600);
        const double remaining = std::fmod(currentValue, 3600);
        const double mi = CMathUtils::trunc(remaining / 60);
        const double se = CMathUtils::trunc(std::fmod(remaining, 60));

        QList<int> parts;
        parts << qRound(hr) << qRound(mi) << qRound(se);
        return parts;
    }

    QString CTime::formattedHrsMinSec() const
    {
        const QList<int> parts = getHrsMinSecParts();
        const QString h = QStringLiteral("00%1").arg(QString::number(parts.at(0))).right(2);
        const QString m = QStringLiteral("00%1").arg(QString::number(parts.at(1))).right(2);
        const QString s = QStringLiteral("00%1").arg(QString::number(parts.at(2))).right(2);

        const QString fs = QStringLiteral("%1:%2:%3").arg(h, m, s);
        if (this->isNegativeWithEpsilonConsidered())
        {
            return QStringLiteral("-").append(fs);
        }
        else
        {
            return fs;
        }
    }

    int CTime::toMs() const
    {
        const double ms = this->valueRounded(CTimeUnit::ms(), 0);
        return static_cast<int>(ms);
    }

    int CTime::toSeconds() const
    {
        const double ms = this->valueRounded(CTimeUnit::s(), 0);
        return static_cast<int>(ms);
    }

    QString CTime::formattedHrsMin() const
    {
        const QList<int> parts = getHrsMinSecParts();
        const QString h = QStringLiteral("00%1").arg(QString::number(parts.at(0))).right(2);
        const QString m = QStringLiteral("00%1").arg(QString::number(parts.at(1))).right(2);
        const QString fs = QStringLiteral("%1:%2").arg(h, m);
        if (this->isNegativeWithEpsilonConsidered())
        {
            return QString("-").append(fs);
        }
        else
        {
            return fs;
        }
    }

    QDateTime CTime::toQDateTime() const
    {
        QDateTime dt = QDateTime::currentDateTimeUtc();
        dt.setTime(this->toQTime());
        return dt;
    }

    QTime CTime::toQTime() const
    {
        const QList<int> parts = this->getHrsMinSecParts();
        const QTime t(parts[0], parts[1], parts[2]);
        return t;
    }
} // ns
