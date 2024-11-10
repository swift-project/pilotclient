// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/timestampbased.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/verify.h"

#include <QByteArray>
#include <QDateTime>

namespace BlackMisc
{
    ITimestampBased::ITimestampBased()
    {}

    ITimestampBased::ITimestampBased(qint64 msSincePoch) : m_timestampMSecsSinceEpoch(msSincePoch)
    {}

    ITimestampBased::ITimestampBased(const QDateTime &timestamp) : m_timestampMSecsSinceEpoch(timestamp.toMSecsSinceEpoch())
    {}

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

        const QString s(removeDateTimeSeparators(yyyyMMddhhmmsszzz));
        const QDateTime dt = parseDateTimeStringOptimized(s);
        this->setUtcTimestamp(dt);
    }

    void ITimestampBased::setUtcTimestamp(const QDateTime &timestamp)
    {
        m_timestampMSecsSinceEpoch = timestamp.isValid() ? timestamp.toMSecsSinceEpoch() : -1;
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

    bool ITimestampBased::isAnyTimestampIndex(int index)
    {
        return (index >= static_cast<int>(IndexUtcTimestamp)) && (index <= static_cast<int>(IndexMSecsSinceEpoch));
    }

    bool ITimestampBased::canHandleIndex(CPropertyIndexRef index)
    {
        if (index.isEmpty()) { return false; }
        const int i = index.frontCasted<int>();
        return isAnyTimestampIndex(i);
    }

    QVariant ITimestampBased::propertyByIndex(CPropertyIndexRef index) const
    {
        if (!index.isEmpty())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexUtcTimestamp: return QVariant::fromValue(this->getUtcTimestamp());
            case IndexMSecsSinceEpoch: return QVariant::fromValue(this->getMSecsSinceEpoch());
            case IndexUtcTimestampFormattedDhms: return QVariant::fromValue(this->getFormattedUtcTimestampDhms());
            case IndexUtcTimestampFormattedHm: return QVariant::fromValue(this->getFormattedUtcTimestampHm());
            case IndexUtcTimestampFormattedHms: return QVariant::fromValue(this->getFormattedUtcTimestampHms());
            case IndexUtcTimestampFormattedYmdhms: return QVariant::fromValue(this->getFormattedUtcTimestampYmdhms());
            case IndexUtcTimestampFormattedYmdhmsz: return QVariant::fromValue(this->getFormattedUtcTimestampYmdhmsz());
            case IndexUtcTimestampFormattedMdhms: return QVariant::fromValue(this->getFormattedUtcTimestampMdhms());
            case IndexUtcTimestampFormattedMdhmsz: return QVariant::fromValue(this->getFormattedUtcTimestampMdhmsz());
            default: break;
            }
        }
        const QString m = QStringLiteral("Cannot handle index %1").arg(index.toQString());
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
        return QVariant::fromValue(m);
    }

    void ITimestampBased::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (!index.isEmpty())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexUtcTimestamp: this->setUtcTimestamp(variant.toDateTime()); return;
            case IndexMSecsSinceEpoch: this->setMSecsSinceEpoch(variant.toLongLong()); return;
            case IndexUtcTimestampFormattedYmdhms:
            case IndexUtcTimestampFormattedYmdhmsz:
            case IndexUtcTimestampFormattedHm:
            case IndexUtcTimestampFormattedHms:
            case IndexUtcTimestampFormattedDhms:
            {
                const QDateTime dt = fromStringUtc(variant.toString());
                m_timestampMSecsSinceEpoch = dt.toMSecsSinceEpoch();
            }
                return;
            default: break;
            }
        }
        const QString m = QStringLiteral("Cannot handle index %1").arg(index.toQString());
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
    }

    int ITimestampBased::comparePropertyByIndex(CPropertyIndexRef index, const ITimestampBased &compareValue) const
    {
        Q_UNUSED(index);
        return Compare::compare(m_timestampMSecsSinceEpoch, compareValue.m_timestampMSecsSinceEpoch);
    }

    void ITimestampBased::updateMissingParts(const ITimestampBased &other)
    {
        if (m_timestampMSecsSinceEpoch < 0)
        {
            m_timestampMSecsSinceEpoch = other.m_timestampMSecsSinceEpoch;
        }
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

    bool ITimestampWithOffsetBased::canHandleIndex(CPropertyIndexRef index)
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
        return QStringLiteral("%1ms").arg(this->getTimeOffsetMs());
    }

    QVariant ITimestampWithOffsetBased::propertyByIndex(CPropertyIndexRef index) const
    {
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
        if (!index.isEmpty())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexOffsetMs:
            {
                return QVariant::fromValue(m_timeOffsetMs);
            }
            case IndexAdjustedMsWithOffset:
            {
                return QVariant::fromValue(this->getAdjustedMSecsSinceEpoch());
            }
            case IndexOffsetWithUnit:
            {
                return QVariant::fromValue(this->getTimeOffsetWithUnit());
            }
            default: break;
            }
        }
        const QString m = QStringLiteral("Cannot handle index %1").arg(index.toQString());
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
        return QVariant();
    }

    void ITimestampWithOffsetBased::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (ITimestampBased::canHandleIndex(index))
        {
            ITimestampBased::setPropertyByIndex(index, variant);
            return;
        }
        if (!index.isEmpty())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexOffsetMs:
            {
                m_timeOffsetMs = variant.value<qint64>();
                return;
            }
            case IndexAdjustedMsWithOffset: return; // read only
            case IndexOffsetWithUnit: return; // read only
            default: break;
            }
        }
        const QString m = QStringLiteral("Cannot handle index %1").arg(index.toQString());
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
    }

    int ITimestampWithOffsetBased::comparePropertyByIndex(CPropertyIndexRef index, const ITimestampWithOffsetBased &compareValue) const
    {
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue); }
        if (!index.isEmpty())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexOffsetWithUnit:
            case IndexOffsetMs:
            {
                return Compare::compare(m_timeOffsetMs, compareValue.m_timeOffsetMs);
            }
            case IndexAdjustedMsWithOffset:
            {
                return Compare::compare(this->getAdjustedMSecsSinceEpoch(), compareValue.getAdjustedMSecsSinceEpoch());
            }
            default: break;
            }
        }
        const QString m = QStringLiteral("Cannot handle index %1").arg(index.toQString());
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
        return 0;
    }
} // namespace
