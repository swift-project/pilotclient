/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DATA_DOWNLOAD_H
#define BLACKCORE_DATA_DOWNLOAD_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/datacache.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

namespace BlackCore
{
    namespace Data
    {
        //! Download locations and versions
        class BLACKCORE_EXPORT CDownload :
            public BlackMisc::CValueObject<CDownload>,
            public BlackMisc::ITimestampBased
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexDownloadUrlsStable = BlackMisc::CPropertyIndex::GlobalIndexCDownload,
                IndexDownloadUrlsBeta,
                IndexLatestVersionStable,
                IndexLastSupportedVersionStable,
                IndexLatestVersionBeta,
                IndexLastSupportedVersionBeta
            };

            //! Default constructor
            CDownload();

            //! Destructor.
            ~CDownload() {}

            //! Download URLs, ie here one can download installer
            const BlackMisc::Network::CUrlList &getDownloadUrlsBeta() const { return m_downloadsBetaUrls; }

            //! Download URLs, ie here one can download installer
            const BlackMisc::Network::CUrlList &getDownloadUrlsStable() const { return m_downloadsStableUrls; }

            //! Download URLs
            BlackMisc::Network::CUrlList getDownloadUrls() const;

            //! Latest version stable channel
            const QString &getLatestVersionStable() const { return m_latestVersionStable; }

            //! Latest version beta channel
            const QString &getLatestVersionBeta() const { return m_latestVersionBeta; }

            //! Latest version
            QString getLatestVersion() const;

            //! Last supported version stable
            const QString &getLastSupportedVersionStable() const { return m_lastSupportedVersionStable; }

            //! Last supported version beta
            const QString &getLastSupportedVersionBeta() const { return m_lastSupportedVersionStable; }

            //! Productive settings?
            bool isDevelopment() const { return m_development; }

            //! Same type
            bool hasSameType(const CDownload &otherDownload) const;

            //! Productive settings?
            void setDevelopment(bool development) { m_development  = development; }

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

            //! To string
            QString convertToQString(const QString &separator, bool i18n = false) const;

            //! \copydoc CValueObject::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CVariant &variant, const BlackMisc::CPropertyIndex &index);

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(BlackCore::Data::CDownload)

            bool                            m_development = false;        //!< for development
            BlackMisc::Network::CUrlList    m_downloadsStableUrls;        //!< Download URLs, here I get the installer
            BlackMisc::Network::CUrlList    m_downloadsBetaUrls;          //!< Download URLs, here I get the installer
            QString                         m_latestVersionStable;        //!< latest version
            QString                         m_lastSupportedVersionStable; //!< last supported version
            QString                         m_latestVersionBeta;          //!< latest version
            QString                         m_lastSupportedVersionBeta;   //!< last supported version
        };

        //! Trait for global setup data
        struct Download : public BlackCore::CDataTrait<CDownload>
        {
            //! Key in data cache
            static const char *key() { return "readers/global/download"; }

            //! Default value
            static const CDownload &defaultValue()
            {
                static const CDownload defaultValue;
                return defaultValue;
            }
        };

    } // ns
} // ns

Q_DECLARE_METATYPE(BlackCore::Data::CDownload)
BLACK_DECLARE_TUPLE_CONVERSION(BlackCore::Data::CDownload, (
                                   attr(o.m_timestampMSecsSinceEpoch),
                                   attr(o.m_development),
                                   attr(o.m_downloadsStableUrls),
                                   attr(o.m_downloadsBetaUrls),
                                   attr(o.m_latestVersionStable),
                                   attr(o.m_lastSupportedVersionStable),
                                   attr(o.m_latestVersionBeta),
                                   attr(o.m_lastSupportedVersionBeta)
                               ))
#endif // guard
