/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/simbriefdata.h"
#include "blackmisc/logcategories.h"
#include <QStringBuilder>

using namespace BlackMisc::Network;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CSimBriefData)

namespace BlackMisc::Aviation
{
    const QStringList &CSimBriefData::getLogCategories()
    {
        static const QStringList cats { CLogCategories::flightPlan() };
        return cats;
    }

    CSimBriefData::CSimBriefData() : m_url("https://www.simbrief.com/api/xml.fetcher.php")
    {}

    CSimBriefData::CSimBriefData(const QString &url, const QString &username) : m_url(url), m_username(username)
    {}

    CUrl CSimBriefData::getUrlAndUsername() const
    {
        CUrl url(this->getUrl());
        if (!m_username.isEmpty()) { url.setQuery("username=" % m_username); }
        return url;
    }

    QVariant CSimBriefData::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexUrl: return QVariant::fromValue(m_url);
        case IndexUsername: return QVariant::fromValue(m_username);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CSimBriefData::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CSimBriefData>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexUrl: m_url = variant.toString(); break;
        case IndexUsername: m_username = variant.toString(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    QString CSimBriefData::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return m_username % " " % m_url;
    }
} // namespace
