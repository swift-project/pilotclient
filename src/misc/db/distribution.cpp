// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/db/distribution.h"

#include <QRegularExpression>
#include <QStringBuilder>

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::network;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::db, CDistribution)

namespace swift::misc::db
{
    CDistribution::CDistribution()
    {}

    CDistribution::CDistribution(const QString &channel, int stability, bool restricted) : m_channel(channel.trimmed().toUpper()), m_stability(stability), m_restricted(restricted)
    {}

    void CDistribution::setChannel(const QString &channel)
    {
        m_channel = channel.trimmed().toUpper();
    }

    void CDistribution::setDownloadUrl(const CUrl &url)
    {
        if (url.isEmpty()) { return; }
        m_downloadUrl = url;
    }

    bool CDistribution::hasDownloadUrl() const
    {
        return !m_downloadUrl.isEmpty();
    }

    CIcons::IconIndex CDistribution::getRestrictionIcon() const
    {
        // StandardIconLockOpen16 is hard to distinguish from closed
        return m_restricted ? CIcons::StandardIconLockClosed16 : CIcons::StandardIconGlobe16;
    }

    bool CDistribution::isStabilitySameOrBetter(const CDistribution &otherDistribution) const
    {
        return m_stability >= otherDistribution.m_stability;
    }

    bool CDistribution::isStabilityBetter(const CDistribution &otherDistribution) const
    {
        return m_stability > otherDistribution.m_stability;
    }

    QString CDistribution::convertToQString(bool i18n) const
    {
        return convertToQString(", ", i18n);
    }

    QString CDistribution::convertToQString(const QString &separator, bool i18n) const
    {
        return u"channel: " %
               this->getChannel() %
               separator %
               u"download URL: " %
               getDownloadUrl().toQString(i18n) %
               separator %
               u"timestamp: " %
               this->getFormattedUtcTimestampYmdhms();
    }

    CIcons::IconIndex CDistribution::toIcon() const
    {
        return this->getRestrictionIcon();
    }

    QVariant CDistribution::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexChannel: return QVariant::fromValue(m_channel);
        case IndexStability: return QVariant::fromValue(m_stability);
        case IndexDownloadUrl: return QVariant::fromValue(m_downloadUrl);
        case IndexRestricted: return QVariant::fromValue(m_restricted);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CDistribution::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CDistribution>();
            return;
        }
        if (IDatastoreObjectWithIntegerKey::canHandleIndex(index))
        {
            IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant);
            return;
        }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexChannel: this->setChannel(variant.value<QString>()); break;
        case IndexStability: m_stability = variant.toInt(); break;
        case IndexDownloadUrl: m_downloadUrl = variant.value<CUrl>(); break;
        case IndexRestricted: m_restricted = variant.toBool(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    const CDistribution &CDistribution::localDeveloperBuild()
    {
        static const CDistribution d = [] {
            CDistribution ld("local developer", 0, true);
            ld.setCurrentUtcTime();
            return ld;
        }();
        return d;
    }

    CDistribution CDistribution::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
    {
        Q_UNUSED(prefix); // not nested

        const QString channel(json.value("channel").toString());
        const bool restricted = json.value("restricted").toBool();
        const int stability = json.value("stability").toInt();
        CDistribution distribution(channel, stability, restricted);
        distribution.setKeyVersionTimestampFromDatabaseJson(json);

        // add the URLs
        for (int i = 0; i < 5; i++)
        {
            const QString key = "url" + QString::number(i);
            const QString url = json.value(key).toString();
            if (url.isEmpty()) { continue; }
            distribution.setDownloadUrl(CUrl(url));
        }
        return distribution;
    }
} // namespace swift::misc::db
