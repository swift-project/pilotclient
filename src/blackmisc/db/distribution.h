/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DB_DISTRIBUTION_H
#define BLACKMISC_DB_DISTRIBUTION_H

#include "blackconfig/buildconfig.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QVersionNumber>
#include <QString>

namespace BlackMisc
{
    namespace Db
    {
        //! Dictionary for files per platform
        using CPlatformDictionary = BlackMisc::CDictionary<QString, QString>;

        //! Distributions for channel
        class BLACKMISC_EXPORT CDistribution :
            public BlackMisc::CValueObject<CDistribution>,
            public BlackMisc::Db::IDatastoreObjectWithIntegerKey
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexChannel = BlackMisc::CPropertyIndex::GlobalIndexCDistribution,
                IndexRestricted,
                IndexDownloadUrls,
                IndexPlatforms,
                IndexPlatformFiles,
            };

            //! Default constructor
            CDistribution();

            //! Constructor
            CDistribution(const QString &channel, bool restricted);

            //! Destructor.
            ~CDistribution() {}

            //! Version channel (Alpha, Beta, Stable ..)
            const QString &getChannel() const { return m_channel; }

            //! Set the channel
            void setChannel(const QString &channel);

            //! Get platforms
            QStringList getPlatforms() const;

            //! Supports platform?
            bool supportsPlatform(const QString &platform) const;

            //! Guess platform for this distribution channel and this version of swift
            QString guessMyPlatform() const;

            //! Version for platform
            QString getVersionString(const QString &platform) const;

            //! Version as QVersion
            QVersionNumber getQVersion(const QString &platform) const;

            //! File for platform
            QString getFilename(const QString &platform) const;

            //! Download URLs, i.e. here one can download installer
            const BlackMisc::Network::CUrlList &getDownloadUrls() const { return m_downloadUrls; }

            //! Add URL, ignored if empty
            void addDownloadUrl(const BlackMisc::Network::CUrl &url);

            //! At least one download URL?
            bool hasDownloadUrls() const;

            //! Restricted channel?
            bool isRestricted() const { return m_restricted; }

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! To string
            QString convertToQString(const QString &separator, bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

            //! Object from database JSON format
            static CDistribution fromDatabaseJson(const QJsonObject &json, const QString &prefix = {});

        private:
            QString                      m_channel;            //!< channel the files belong to
            bool                         m_restricted = false; //!< restricted access (i.e. password for download needed)
            BlackMisc::Network::CUrlList m_downloadUrls;       //!< download URLs, here I get the installer
            CPlatformDictionary          m_platformFiles;      //!< the latest file version per platform
            CPlatformDictionary          m_platformVersions;   //!< the version per platform

            //! Extract version number from a file name
            static QVersionNumber versionNumberFromFilename(const QString &filename);

            //! Find the file representing the latest version
            static QPair<QString, QVersionNumber> findLatestVersion(const QStringList &filenames);

            BLACK_METACLASS(
                CDistribution,
                BLACK_METAMEMBER(dbKey),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(channel),
                BLACK_METAMEMBER(downloadUrls),
                BLACK_METAMEMBER(platformFiles, 0, DisabledForComparison | DisabledForHashing),
                BLACK_METAMEMBER(platformVersions, 0, DisabledForComparison | DisabledForHashing)
            );
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Db::CDistribution)
Q_DECLARE_METATYPE(BlackMisc::Db::CPlatformDictionary)

#endif // guard
