/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DATA_UPDATEINFO_H
#define BLACKCORE_DATA_UPDATEINFO_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/datacache.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

namespace BlackCore
{
    namespace Data
    {
        //! Download locations and versions
        class BLACKCORE_EXPORT CUpdateInfo :
            public BlackMisc::CValueObject<CUpdateInfo>,
            public BlackMisc::ITimestampBased
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexDownloadUrlsStable = BlackMisc::CPropertyIndex::GlobalIndexCUpdateInfo,
                IndexDownloadUrlsBeta,
                IndexLatestVersionStable,
                IndexLastSupportedVersionStable,
                IndexLatestVersionBeta,
                IndexLastSupportedVersionBeta
            };

            //! Default constructor
            CUpdateInfo();

            //! Destructor.
            ~CUpdateInfo() {}

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
            bool hasSameType(const CUpdateInfo &otherDownload) const;

            //! Productive settings?
            void setDevelopment(bool development) { m_development  = development; }

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! To string
            QString convertToQString(const QString &separator, bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

        private:
            bool                            m_development = false;        //!< for development
            BlackMisc::Network::CUrlList    m_downloadsStableUrls;        //!< Download URLs, here I get the installer
            BlackMisc::Network::CUrlList    m_downloadsBetaUrls;          //!< Download URLs, here I get the installer
            QString                         m_latestVersionStable;        //!< latest version
            QString                         m_lastSupportedVersionStable; //!< last supported version
            QString                         m_latestVersionBeta;          //!< latest version
            QString                         m_lastSupportedVersionBeta;   //!< last supported version

            BLACK_METACLASS(
                CUpdateInfo,
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(development),
                BLACK_METAMEMBER(downloadsStableUrls),
                BLACK_METAMEMBER(downloadsBetaUrls),
                BLACK_METAMEMBER(latestVersionStable),
                BLACK_METAMEMBER(lastSupportedVersionStable),
                BLACK_METAMEMBER(latestVersionBeta),
                BLACK_METAMEMBER(lastSupportedVersionBeta)
            );
        };

        //! Trait for global setup data
        struct UpdateInfo : public BlackMisc::CDataTrait<CUpdateInfo>
        {
            //! Key in data cache
            static const char *key() { return "version"; }

            //! First load is synchronous
            static bool isPinned() { return true; }

            //! Default value
            static const CUpdateInfo &defaultValue()
            {
                static const CUpdateInfo defaultValue;
                return defaultValue;
            }
        };

    } // ns
} // ns

Q_DECLARE_METATYPE(BlackCore::Data::CUpdateInfo)

#endif // guard
