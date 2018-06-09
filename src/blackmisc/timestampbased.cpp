/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/comparefunctions.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/variant.h"
#include "blackmisc/verify.h"

#include <QByteArray>
#include <QDate>
#include <QTime>
#include <Qt>

namespace BlackMisc
{
    ITimestampBased::ITimestampBased()
    { }

    ITimestampBased::ITimestampBased(qint64 msSincePoch) : m_timestampMSecsSinceEpoch(msSincePoch)
    { }

    ITimestampBased::ITimestampBased(const QDateTime &timestamp) : m_timestampMSecsSinceEpoch(timestamp.toMSecsSinceEpoch())
    { }

    QDateTime ITimestampBased::getUtcTimestamp() const
    {
        if (m_timestampMSecsSinceEpoch < 0) { return QDateTime(); }
        return QDateTime::fromMSecsSinceEpoch(m_timestampMSecsSinceEpoch);
    }

    void ITimestampBased::setTimestampToNull()
    {
        m_timestampMSecsSinceEpoch = -1;
    }

    void ITimestampBased::setByYearMonthDayHourMinute(const QString &yyyyMMddhhmmsszzz)
    {
        // yyyy MM dd hh mm ss zzz
        // 0123 45 67 89 01 23 456
        // 1234 56 78 90 12 34 567

        QString s(yyyyMMddhhmmsszzz);
        s.remove(':').remove(' ').remove('-').remove('.'); // plain vanilla string
        int year(s.leftRef(4).toInt());
        int month(s.midRef(4, 2).toInt());
        int day(s.midRef(6, 2).toInt());
        QDate date;
        date.setDate(year, month, day);
        QDateTime dt;
        dt.setOffsetFromUtc(0);
        dt.setDate(date);
        if (s.length() < 12)
        {
            this->setUtcTimestamp(dt);
            return;
        }

        QTime t;
        int hour(s.midRef(8, 2).toInt());
        int minute(s.midRef(10, 2).toInt());
        int second(s.length() < 14 ? 0 : s.midRef(12, 2).toInt());
        int ms(s.length() < 17 ? 0 : s.rightRef(3).toInt());

        t.setHMS(hour, minute, second, ms);
        dt.setTime(t);
        this->setUtcTimestamp(dt);
    }

    void ITimestampBased::setUtcTimestamp(const QDateTime &timestamp)
    {
        if (timestamp.isValid())
        {
            m_timestampMSecsSinceEpoch = timestamp.toMSecsSinceEpoch();
        }
        else
        {
            m_timestampMSecsSinceEpoch = -1; // invalid
        }
    }

    bool ITimestampBased::isNewerThan(const ITimestampBased &otherTimestampObj) const
    {
        return m_timestampMSecsSinceEpoch > otherTimestampObj.m_timestampMSecsSinceEpoch;
    }

    bool ITimestampBased::isNewerThan(qint64 mSecsSinceEpoch) const
    {
        return m_timestampMSecsSinceEpoch > mSecsSinceEpoch;
    }

    bool ITimestampBased::isOlderThan(const ITimestampBased &otherTimestampObj) const
    {
        return m_timestampMSecsSinceEpoch < otherTimestampObj.m_timestampMSecsSinceEpoch;
    }

    bool ITimestampBased::isOlderThan(qint64 mSecsSinceEpoch) const
    {
        return m_timestampMSecsSinceEpoch < mSecsSinceEpoch;
    }

    bool ITimestampBased::isOlderThanNowMinusOffset(int offsetMs) const
    {
        if (offsetMs <= 0) { return false; }
        return m_timestampMSecsSinceEpoch < (QDateTime::currentMSecsSinceEpoch() - offsetMs);
    }

    bool ITimestampBased::isSame(const ITimestampBased &otherTimestampObj) const
    {
        return m_timestampMSecsSinceEpoch == otherTimestampObj.m_timestampMSecsSinceEpoch;
    }

    qint64 ITimestampBased::msecsTo(const ITimestampBased &otherTimestampObj) const
    {
        return otherTimestampObj.m_timestampMSecsSinceEpoch - m_timestampMSecsSinceEpoch;
    }

    qint64 ITimestampBased::absMsecsTo(const ITimestampBased &otherTimestampObj) const
    {
        qint64 dt = this->msecsTo(otherTimestampObj);
        return dt > 0 ? dt : dt * -1;
    }

    qint64 ITimestampBased::msecsToNow() const
    {
        if (m_timestampMSecsSinceEpoch < 0) return QDateTime::currentMSecsSinceEpoch();
        return QDateTime::currentMSecsSinceEpoch() - m_timestampMSecsSinceEpoch;
    }

    void ITimestampBased::setCurrentUtcTime()
    {
        m_timestampMSecsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
    }

    void ITimestampBased::addMsecs(qint64 ms)
    {
        m_timestampMSecsSinceEpoch += ms;
    }

    QString ITimestampBased::getFormattedUtcTimestampMdhms() const
    {
        return this->hasValidTimestamp() ?
               this->getUtcTimestamp().toString("MM-dd hh:mm:ss") :
               "";
    }

    QString ITimestampBased::getFormattedUtcTimestampMdhmsz() const
    {
        return this->hasValidTimestamp() ?
               this->getUtcTimestamp().toString("MM-dd hh:mm:ss.zzz") :
               "";
    }

    QString ITimestampBased::getFormattedUtcTimestampDhms() const
    {
        return this->hasValidTimestamp() ?
               this->getUtcTimestamp().toString("dd hh:mm:ss") :
               "";
    }

    QString ITimestampBased::getFormattedUtcTimestampHms() const
    {
        return this->hasValidTimestamp() ?
               this->getUtcTimestamp().toString("hh:mm:ss") :
               "";
    }

    QString ITimestampBased::getFormattedUtcTimestampHmsz() const
    {
        return this->hasValidTimestamp() ?
               this->getUtcTimestamp().toString("hh:mm:ss.zzz") :
               "";
    }

    QString ITimestampBased::getFormattedUtcTimestampHm() const
    {
        return this->hasValidTimestamp() ?
               this->getUtcTimestamp().toString("hh::mm") :
               "";
    }

    QString ITimestampBased::getFormattedUtcTimestampYmdhms() const
    {
        return this->hasValidTimestamp() ?
               this->getUtcTimestamp().toString("yyyy-MM-dd HH:mm:ss") :
               "";
    }

    QString ITimestampBased::getFormattedUtcTimestampYmdhmsz() const
    {
        return this->hasValidTimestamp() ?
               this->getUtcTimestamp().toString("yyyy-MM-dd HH:mm:ss.zzz") :
               "";
    }

    bool ITimestampBased::hasValidTimestamp() const
    {
        return m_timestampMSecsSinceEpoch >= 0;
    }

    bool ITimestampBased::canHandleIndex(const CPropertyIndex &index)
    {
        if (index.isEmpty()) { return false; }
        const int i = index.frontCasted<int>();
        return (i >= static_cast<int>(IndexUtcTimestamp)) && (i <= static_cast<int>(IndexMSecsSinceEpoch));
    }

    CVariant ITimestampBased::propertyByIndex(const CPropertyIndex &index) const
    {
        if (!index.isEmpty())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexUtcTimestamp: return CVariant::fromValue(this->getUtcTimestamp());
            case IndexMSecsSinceEpoch: return CVariant::fromValue(this->getMSecsSinceEpoch());
            case IndexUtcTimestampFormattedDhms: return CVariant::fromValue(this->getFormattedUtcTimestampDhms());
            case IndexUtcTimestampFormattedHm: return CVariant::fromValue(this->getFormattedUtcTimestampHm());
            case IndexUtcTimestampFormattedHms: return CVariant::fromValue(this->getFormattedUtcTimestampHms());
            case IndexUtcTimestampFormattedYmdhms: return CVariant::fromValue(this->getFormattedUtcTimestampYmdhms());
            case IndexUtcTimestampFormattedYmdhmsz: return CVariant::fromValue(this->getFormattedUtcTimestampYmdhmsz());
            case IndexUtcTimestampFormattedMdhms: return CVariant::fromValue(this->getFormattedUtcTimestampMdhms());
            case IndexUtcTimestampFormattedMdhmsz: return CVariant::fromValue(this->getFormattedUtcTimestampMdhmsz());
            default: break;
            }
        }
        const QString m = QString("Cannot handle index %1").arg(index.toQString());
        BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
        return CVariant::fromValue(m);
    }

    void ITimestampBased::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        if (!index.isEmpty())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexUtcTimestamp: this->setUtcTimestamp(variant.toDateTime()); return;
            case IndexMSecsSinceEpoch: this->setMSecsSinceEpoch(variant.toQInt64()); return;
            case IndexUtcTimestampFormattedYmdhms:
            case IndexUtcTimestampFormattedYmdhmsz:
            case IndexUtcTimestampFormattedHm:
            case IndexUtcTimestampFormattedHms:
            case IndexUtcTimestampFormattedDhms:
                {
                    const QDateTime dt = QDateTime::fromString(variant.toQString());
                    m_timestampMSecsSinceEpoch = dt.toMSecsSinceEpoch();
                }
                return;
            default: break;
            }
        }
        const QString m = QString("Cannot handle index %1").arg(index.toQString());
        BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
    }

    int ITimestampBased::comparePropertyByIndex(const CPropertyIndex &index, const ITimestampBased &compareValue) const
    {
        Q_UNUSED(index);
        return Compare::compare(m_timestampMSecsSinceEpoch, compareValue.m_timestampMSecsSinceEpoch);
    }

    QString ITimestampWithOffsetBased::getTimestampAndOffset(bool formatted) const
    {
        static const QString ts("%1 (%2)");
        if (!this->hasValidTimestamp()) { return ts.arg("-", this->getTimeOffsetWithUnit()); }
        if (formatted) { return ts.arg(this->getFormattedUtcTimestampHms(), this->getTimeOffsetWithUnit()); }
        return ts.arg(m_timeOffsetMs).arg(this->getTimeOffsetWithUnit());
    }

    QString ITimestampWithOffsetBased::getFormattedTimestampAndOffset(bool includeRawTimestamp) const
    {
        if (!includeRawTimestamp) { return this->getTimestampAndOffset(true); }
        static const QString ts("%1/%2 (%3)");
        if (!this->hasValidTimestamp()) { return ts.arg("-", "-", this->getTimeOffsetWithUnit()); }
        return ts.arg(this->getFormattedUtcTimestampHmsz()).arg(m_timestampMSecsSinceEpoch).arg(this->getTimeOffsetWithUnit());
    }

    bool ITimestampWithOffsetBased::isNewerThanAdjusted(const ITimestampWithOffsetBased &otherTimestampObj) const
    {
        return this->getAdjustedMSecsSinceEpoch() > otherTimestampObj.getAdjustedMSecsSinceEpoch();
    }

    bool ITimestampWithOffsetBased::isNewerThanAdjusted(qint64 mSecsSinceEpoch) const
    {
        return this->getAdjustedMSecsSinceEpoch() > mSecsSinceEpoch;
    }

    bool ITimestampWithOffsetBased::isOlderThanAdjusted(const ITimestampWithOffsetBased &otherTimestampObj) const
    {
        return this->getAdjustedMSecsSinceEpoch() < otherTimestampObj.getAdjustedMSecsSinceEpoch();
    }

    bool ITimestampWithOffsetBased::isOlderThanAdjusted(qint64 mSecsSinceEpoch) const
    {
        return this->getAdjustedMSecsSinceEpoch() < mSecsSinceEpoch;
    }

    bool ITimestampWithOffsetBased::canHandleIndex(const CPropertyIndex &index)
    {
        if (ITimestampBased::canHandleIndex(index)) { return true; }
        if (index.isEmpty()) { return false; }
        const int i = index.frontCasted<int>();
        return (i >= static_cast<int>(IndexOffsetMs)) && (i <= static_cast<int>(IndexOffsetWithUnit));
    }

    bool ITimestampWithOffsetBased::hasNonZeroOffsetTime() const
    {
        return m_timeOffsetMs != 0;
    }

    void ITimestampWithOffsetBased::addMsecsToOffsetTime(qint64 msToAdd)
    {
        m_timeOffsetMs += msToAdd;
    }

    QString ITimestampWithOffsetBased::getTimeOffsetWithUnit() const
    {
        static const QString os("%1ms");
        return os.arg(this->getTimeOffsetMs());
    }

    CVariant ITimestampWithOffsetBased::propertyByIndex(const CPropertyIndex &index) const
    {
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
        if (!index.isEmpty())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexOffsetMs: { return QVariant::fromValue(m_timeOffsetMs); }
            case IndexAdjustedMsWithOffset: { return QVariant::fromValue(this->getAdjustedMSecsSinceEpoch()); }
            case IndexOffsetWithUnit: { return QVariant::fromValue(this->getTimeOffsetWithUnit()); }
            default: break;
            }
        }
        const QString m = QString("Cannot handle index %1").arg(index.toQString());
        BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
        return CVariant();
    }

    void ITimestampWithOffsetBased::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(index, variant); return; }
        if (!index.isEmpty())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexOffsetMs: { m_timeOffsetMs = variant.value<qint64>(); return; }
            case IndexAdjustedMsWithOffset: return; // read only
            case IndexOffsetWithUnit: return; // read only
            default: break;
            }
        }
        const QString m = QString("Cannot handle index %1").arg(index.toQString());
        BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
    }

    int ITimestampWithOffsetBased::comparePropertyByIndex(const CPropertyIndex &index, const ITimestampWithOffsetBased &compareValue) const
    {
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue); }
        if (!index.isEmpty())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexOffsetWithUnit:
            case IndexOffsetMs: { return Compare::compare(m_timeOffsetMs, compareValue.m_timeOffsetMs); }
            case IndexAdjustedMsWithOffset: { return Compare::compare(this->getAdjustedMSecsSinceEpoch(), compareValue.getAdjustedMSecsSinceEpoch()); }
            default: break;
            }
        }
        const QString m = QString("Cannot handle index %1").arg(index.toQString());
        BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
        return 0;
    }
} // namespace
