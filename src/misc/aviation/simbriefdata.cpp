// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/simbriefdata.h"

#include <QStringBuilder>

#include "misc/logcategories.h"

using namespace swift::misc::network;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CSimBriefData)

namespace swift::misc::aviation
{
    const QStringList &CSimBriefData::getLogCategories()
    {
        static const QStringList cats { CLogCategories::flightPlan() };
        return cats;
    }

    CSimBriefData::CSimBriefData() : m_url("https://www.simbrief.com/api/xml.fetcher.php") {}

    CSimBriefData::CSimBriefData(const QString &url, const QString &username) : m_url(url), m_username(username) {}

    CUrl CSimBriefData::getUrlAndUsername() const
    {
        CUrl url(this->getUrl());
        if (!m_username.isEmpty()) { url.setQuery("username=" % m_username); }
        return url;
    }

    QVariant CSimBriefData::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const auto i = index.frontCasted<ColumnIndex>();
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
        const auto i = index.frontCasted<ColumnIndex>();
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
} // namespace swift::misc::aviation
