// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/urllog.h"
#include "misc/network/networkutils.h"
#include "misc/propertyindexref.h"
#include "misc/stringutils.h"

#include <QJsonValue>
#include <QtGlobal>

#include <atomic>

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::network, CUrlLog)

namespace swift::misc::network
{
    CUrlLog::CUrlLog(const CUrl &url) : ITimestampBased(), m_id(uniqueId()), m_url(url)
    {
        ITimestampBased::setCurrentUtcTime();
    }

    void CUrlLog::setResponseTimestampToNow()
    {
        m_responseTimeMSecsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
        m_responseTimeMs = m_responseTimeMSecsSinceEpoch - ITimestampBased::getMSecsSinceEpoch();
    }

    bool CUrlLog::isPending() const
    {
        return m_responseTimeMs < 0;
    }

    QVariant CUrlLog::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexId: return QVariant::fromValue(m_id);
        case IndexSuccess: return QVariant::fromValue(m_success);
        case IndexUrl: return this->m_url.propertyByIndex(index.copyFrontRemoved());
        case IndexResponseTimestamp: return QVariant::fromValue(this->getResponseTimestamp());
        case IndexResponseTime: return QVariant::fromValue(m_responseTimeMs);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CUrlLog::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CUrlLog>();
            return;
        }
        if (ITimestampBased::canHandleIndex(index))
        {
            ITimestampBased::setPropertyByIndex(index, variant);
            return;
        }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexId: m_id = variant.toInt(); break;
        case IndexSuccess: m_success = variant.toBool(); break;
        case IndexUrl: m_url.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexResponseTime: this->setResponseTimestampToNow(); break; // a bit unusual
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    QString CUrlLog::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        static const QString s("Id: %1, success: %2 response: %3ms, started: %4 ended: %5");
        return s.arg(m_id).arg(boolToYesNo(m_success)).arg(m_responseTimeMs).arg(this->getMSecsSinceEpoch()).arg(m_responseTimeMSecsSinceEpoch);
    }

    const char *CUrlLog::propertyNameId()
    {
        static const QByteArray p(QString("urlLogId").toLatin1());
        return p.constData();
    }

    int CUrlLog::uniqueId()
    {
        static std::atomic_int s_id { 1 }; // 0 means default in property system, so I start with 1
        const int id = s_id++;
        return id;
    }
} // namespace
