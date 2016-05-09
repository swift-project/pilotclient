/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "updateinfo.h"
#include "blackmisc/buildconfig.h"
#include <QStringList>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackCore
{
    namespace Data
    {
        CUpdateInfo::CUpdateInfo() :
            ITimestampBased(0),
            m_downloadsStableUrls(QStringList {"http://swift-project.org/"}),
            m_downloadsBetaUrls(QStringList {"http://swift-project.org/"}),
            m_latestVersionStable(CVersion::version()),
            m_lastSupportedVersionStable("0.6"),
            m_latestVersionBeta(CVersion::version()),
            m_lastSupportedVersionBeta("0.6")
        { }

        CUrlList CUpdateInfo::getDownloadUrls() const
        {
            return CBuildConfig::isBetaTest() ? getDownloadUrlsBeta() : getDownloadUrlsStable();
        }

        QString CUpdateInfo::getLatestVersion() const
        {
            return CBuildConfig::isBetaTest() ? getLatestVersionBeta() : getLatestVersionStable();
        }

        bool CUpdateInfo::hasSameType(const CUpdateInfo &otherDownload) const
        {
            return this->isDevelopment() == otherDownload.isDevelopment();
        }

        QString CUpdateInfo::convertToQString(bool i18n) const
        {
            return convertToQString(", ", i18n);
        }

        QString CUpdateInfo::convertToQString(const QString &separator, bool i18n) const
        {
            QString s("timestamp: ");
            s.append(this->getFormattedUtcTimestampYmdhms());
            s.append(separator);
            s.append("Download URLs (stable): ");
            s.append(getDownloadUrlsStable().toQString(i18n));
            s.append(separator);
            s.append("Download URLs (beta): ");
            s.append(getDownloadUrlsBeta().toQString(i18n));
            s.append(separator);
            s.append("Latest version (stable): ");
            s.append(getLatestVersionStable());
            s.append(separator);
            s.append("Latest version (beta): ");
            s.append(getLatestVersionBeta());
            s.append(separator);
            s.append("Latest supported version (stable): ");
            s.append(getLastSupportedVersionStable());
            s.append(separator);
            s.append("Latest supported version (beta): ");
            s.append(getLastSupportedVersionBeta());
            s.append(separator);
            return s;
        }

        CVariant CUpdateInfo::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDownloadUrlsStable:
                return CVariant::fromValue(this->m_downloadsStableUrls);
            case IndexDownloadUrlsBeta:
                return CVariant::fromValue(this->m_downloadsBetaUrls);
            case IndexLastSupportedVersionStable:
                return CVariant::fromValue(this->m_lastSupportedVersionStable);
            case IndexLatestVersionStable:
                return CVariant::fromValue(this->m_latestVersionStable);
            case IndexLastSupportedVersionBeta:
                return CVariant::fromValue(this->m_lastSupportedVersionBeta);
            case IndexLatestVersionBeta:
                return CVariant::fromValue(this->m_latestVersionBeta);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CUpdateInfo::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CUpdateInfo>(); return; }
            if (ITimestampBased::canHandleIndex(index))
            {
                ITimestampBased::setPropertyByIndex(index, variant);
                return;
            }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDownloadUrlsStable:
                this->m_downloadsStableUrls = variant.value<CUrlList>();
                break;
            case IndexDownloadUrlsBeta:
                this->m_downloadsBetaUrls = variant.value<CUrlList>();
                break;
            case IndexLastSupportedVersionStable:
                this->m_lastSupportedVersionStable = variant.toQString();
                break;
            case IndexLatestVersionStable:
                this->m_latestVersionStable = variant.toQString();
                break;
            case IndexLastSupportedVersionBeta:
                this->m_lastSupportedVersionBeta = variant.toQString();
                break;
            case IndexLatestVersionBeta:
                this->m_latestVersionBeta = variant.toQString();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

    } // ns
} // ns
