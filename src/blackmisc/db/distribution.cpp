/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "distribution.h"
#include <QRegularExpression>
#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Db
    {
        CDistribution::CDistribution()
        { }

        CDistribution::CDistribution(const QString &channel, int stability, bool restricted) :
            m_channel(channel.trimmed().toUpper()), m_stability(stability), m_restricted(restricted)
        { }

        void CDistribution::setChannel(const QString &channel)
        {
            m_channel = channel.trimmed().toUpper();
        }

        void CDistribution::addDownloadUrl(const CUrl &url)
        {
            if (url.isEmpty()) { return; }
            m_downloadUrls.push_back(url);
        }

        bool CDistribution::hasDownloadUrls() const
        {
            return !m_downloadUrls.isEmpty();
        }

        CIcon CDistribution::getRestrictionIcon() const
        {
            return CIcon::iconByIndex(m_restricted ? CIcons::StandardIconLockClosed16 : CIcons::StandardIconLockOpen16);
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
            return QLatin1String("channel: ") %
                   this->getChannel() %
                   separator %
                   QLatin1String("download URLs: ") %
                   getDownloadUrls().toQString(i18n) %
                   separator %
                   QLatin1String("timestamp: ") %
                   this->getFormattedUtcTimestampYmdhms();
        }

        CIcon CDistribution::toIcon() const
        {
            return this->getRestrictionIcon();
        }

        CVariant CDistribution::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexChannel: return CVariant::fromValue(m_channel);
            case IndexStability : return CVariant::fromValue(m_stability);
            case IndexDownloadUrls: return CVariant::fromValue(m_downloadUrls);
            case IndexRestricted: return CVariant::fromValue(m_restricted);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CDistribution::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CDistribution>(); return; }
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
            case IndexDownloadUrls: m_downloadUrls = variant.value<CUrlList>(); break;
            case IndexRestricted: m_restricted = variant.toBool(); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        CDistribution CDistribution::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            Q_UNUSED(prefix); // not nested

            const QString channel(json.value("channel").toString());
            const bool restricted = json.value("restricted").toBool();
            const int stability = json.value("stability").toInt();
            CDistribution distribution(channel, stability, restricted);
            distribution.setKeyAndTimestampFromDatabaseJson(json);

            // add the URLs
            for (int i = 0; i < 5; i++)
            {
                const QString key = "url" + QString::number(i);
                const QString url = json.value(key).toString();
                if (url.isEmpty()) { continue; }
                distribution.addDownloadUrl(CUrl(url));
            }
            return distribution;
        }
    } // ns
} // ns
