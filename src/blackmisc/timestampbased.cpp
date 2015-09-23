/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "timestampbased.h"
#include "variant.h"

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
        return QDateTime::fromMSecsSinceEpoch(this->m_timestampMSecsSinceEpoch, Qt::UTC);
    }

    void ITimestampBased::setUtcTimestamp(const QDateTime &timestamp)
    {
        this->m_timestampMSecsSinceEpoch = timestamp.toMSecsSinceEpoch();
    }

    bool ITimestampBased::isNewerThan(const ITimestampBased &otherTimestampObj) const
    {
        return this->m_timestampMSecsSinceEpoch > otherTimestampObj.m_timestampMSecsSinceEpoch;
    }

    bool ITimestampBased::isNewerThan(qint64 mSecsSinceEpoch) const
    {
        return this->m_timestampMSecsSinceEpoch > mSecsSinceEpoch;
    }

    bool ITimestampBased::isOlderThan(const ITimestampBased &otherTimestampObj) const
    {
        return this->m_timestampMSecsSinceEpoch < otherTimestampObj.m_timestampMSecsSinceEpoch;
    }

    bool ITimestampBased::isOlderThan(qint64 mSecsSinceEpoch) const
    {
        return this->m_timestampMSecsSinceEpoch < mSecsSinceEpoch;
    }

    bool ITimestampBased::isOlderThanNowMinusOffset(int offsetMs) const
    {
        if (offsetMs <= 0) { return false; }
        return this->m_timestampMSecsSinceEpoch < (QDateTime::currentMSecsSinceEpoch() - offsetMs);
    }

    bool ITimestampBased::isSame(const ITimestampBased &otherTimestampObj) const
    {
        return this->m_timestampMSecsSinceEpoch == otherTimestampObj.m_timestampMSecsSinceEpoch;
    }

    qint64 ITimestampBased::msecsTo(const ITimestampBased &otherTimestampObj) const
    {
        return otherTimestampObj.m_timestampMSecsSinceEpoch - this->m_timestampMSecsSinceEpoch;
    }

    qint64 ITimestampBased::absMsecsTo(const ITimestampBased &otherTimestampObj) const
    {
        qint64 dt = this->msecsTo(otherTimestampObj);
        return dt > 0 ? dt : dt * -1;
    }

    void ITimestampBased::setCurrentUtcTime()
    {
        this->m_timestampMSecsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
    }

    QString ITimestampBased::getFormattedUtcTimestampDhms() const
    {
        return this->getUtcTimestamp().toString("dd hh:mm:ss");
    }

    QString ITimestampBased::getFormattedUtcTimestampHms() const
    {
        return this->getUtcTimestamp().toString("hh:mm:ss");
    }

    QString ITimestampBased::getFormattedUtcTimestampHm() const
    {
        return this->getUtcTimestamp().toString("hh::mm");
    }

    QString ITimestampBased::getFormattedUtcTimestampYmdhms() const
    {
        return this->getUtcTimestamp().toString("yyyy-MM-dd HH:mm:ss");
    }

    QString ITimestampBased::getFormattedUtcTimestampYmdhmsz() const
    {
        return this->getUtcTimestamp().toString("yyyy-MM-dd HH:mm:ss.zzz");
    }

    bool ITimestampBased::canHandleIndex(const CPropertyIndex &index)
    {
        int i = index.frontCasted<int>();
        return (i >= static_cast<int>(IndexUtcTimestamp)) && (i <= static_cast<int>(IndexMSecsSinceEpoch));
    }

    CVariant ITimestampBased::propertyByIndex(const CPropertyIndex &index) const
    {
        if (!index.isEmpty())
        {
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexUtcTimestamp:
                return CVariant::fromValue(this->getUtcTimestamp());
            case IndexMSecsSinceEpoch:
                return CVariant::fromValue(this->getMSecsSinceEpoch());
            case IndexUtcTimestampFormattedDhms:
                return CVariant::fromValue(this->getFormattedUtcTimestampDhms());
            case IndexUtcTimestampFormattedHm:
                return CVariant::fromValue(this->getFormattedUtcTimestampHm());
            case IndexUtcTimestampFormattedHms:
                return CVariant::fromValue(this->getFormattedUtcTimestampHms());
            case IndexUtcTimestampFormattedYmdhms:
                return CVariant::fromValue(this->getFormattedUtcTimestampYmdhms());
            case IndexUtcTimestampFormattedYmdhmsz:
                return CVariant::fromValue(this->getFormattedUtcTimestampYmdhmsz());
            default:
                break;
            }
        }
        const QString m = QString("Cannot handle index %1").arg(index.toQString());
        Q_ASSERT_X(false, "propertyByIndex", m.toLocal8Bit().constData());
        return CVariant::fromValue(m);
    }

    void ITimestampBased::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
    {
        if (!index.isEmpty())
        {
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexUtcTimestamp:
                this->setUtcTimestamp(variant.toDateTime());
                return;
            case IndexMSecsSinceEpoch:
                this->setMSecsSinceEpoch(variant.toInt());
                return;
            case IndexUtcTimestampFormattedYmdhms:
            case IndexUtcTimestampFormattedYmdhmsz:
            case IndexUtcTimestampFormattedHm:
            case IndexUtcTimestampFormattedHms:
            case IndexUtcTimestampFormattedDhms:
                {
                    QDateTime dt = QDateTime::fromString(variant.toQString());
                    this->m_timestampMSecsSinceEpoch = dt.toMSecsSinceEpoch();
                }
            default:
                break;
            }
        }
        const QString m = QString("Cannot handle index %1").arg(index.toQString());
        Q_ASSERT_X(false, "setPropertyByIndex", m.toLocal8Bit().constData());
    }

} // namespace
