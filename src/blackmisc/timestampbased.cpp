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

    QDateTime ITimestampBased::getUtcTimestamp() const
    {
        return QDateTime::fromMSecsSinceEpoch(this->m_timestampMSecsSinceEpoch, Qt::UTC);
    }

    QString ITimestampBased::getFormattedUtcTimestamp() const
    {
        return this->getUtcTimestamp().toString("dd hh:mm:ss");
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

    qint64 ITimestampBased::msecsToAbs(const ITimestampBased &otherTimestampObj) const
    {
        qint64 dt = this->msecsTo(otherTimestampObj);
        return dt > 0 ? dt : dt * -1;
    }

    void ITimestampBased::setCurrentUtcTime()
    {
        this->m_timestampMSecsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
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
            case IndexUtcTimestampFormatted:
                return CVariant::fromValue(this->getFormattedUtcTimestamp());
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
            case IndexUtcTimestampFormatted:
            default:
                break;
            }
        }
        const QString m = QString("Cannot handle index %1").arg(index.toQString());
        Q_ASSERT_X(false, "setPropertyByIndex", m.toLocal8Bit().constData());
    }

} // namespace
