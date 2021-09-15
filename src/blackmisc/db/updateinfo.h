/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DB_UPDATEINFO_H
#define BLACKMISC_DB_UPDATEINFO_H

#include "artifactlist.h"
#include "distributionlist.h"
#include "blackmisc/datacache.h"
#include "blackmisc/valueobject.h"
#include <QStringList>

namespace BlackMisc::Db
{
    //! Update info, i.e. artifacts and distributions
    //! \sa CArtifact
    //! \sa CDistribution
    class BLACKMISC_EXPORT CUpdateInfo : public CValueObject<CUpdateInfo>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexArtifactsPilotClient = CPropertyIndexRef::GlobalIndexCUpdateInfo,
            IndexArtifactsXSwiftBus,
            IndexDistributions
        };

        //! Constructor
        CUpdateInfo() {}

        //! Constructor
        CUpdateInfo(const CArtifactList &artifacts, const CDistributionList &distributions);

        //! Destructor.
        ~CUpdateInfo() {}

        //! Artifacts (pilot client)
        const CArtifactList &getArtifactsPilotClient() const { return m_artifactsPilotClient; }

        //! Artifacts for current platform
        //! \note sorted by version
        CArtifactList getArtifactsPilotClientForCurrentPlatform() const;

        //! Artifacts (XSwiftBus)
        const CArtifactList &getArtifactsXSwiftBus() const { return m_artifactsXSwiftBus; }

        //! Artifacts (XSwiftBus)
        CArtifactList getArtifactsXSwiftBusLatestVersionFirst() const;

        //! Artifacts (XSwiftBus)
        CArtifactList getArtifactsXSwiftBusOldestVersionFirst() const;

        //! Artifacts for current platform
        //! \note sorted by version
        CArtifactList getArtifactsXSwiftBusForCurrentPlatform() const;

        //! Distributions (all)
        const CDistributionList &getDistributions() const { return m_distributions; }

        //! Distributions for current platform
        CDistributionList getDistributionsPilotClientForCurrentPlatform() const;

        //! Own distribution
        CDistribution anticipateOwnDistribution() const;

        //! Default channel, OS
        QStringList anticipateMyDefaultChannelAndPlatform() const;

        //! Empty (no data)
        bool isEmpty() const { return m_artifactsPilotClient.isEmpty() && m_distributions.isEmpty(); }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! To string
        QString convertToQString(const QString &separator, bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! Object from database JSON format
        static CUpdateInfo fromDatabaseJson(const QJsonObject &json, const QString &prefix = {});

        //! Object from database JSOn format
        static CUpdateInfo fromDatabaseJson(const QString &jsonString);

        //! Object from GitHub Releases REST API JSON format
        static CUpdateInfo fromGitHubReleasesJson(const QByteArray &jsonData);

    private:
        CArtifactList m_artifactsPilotClient; //!< artifacts pilot client
        CArtifactList m_artifactsXSwiftBus;   //!< artifacts XSwiftBus
        CDistributionList m_distributions;    //!< all distributions (for any artifacts)

        BLACK_METACLASS(
            CUpdateInfo,
            BLACK_METAMEMBER(artifactsPilotClient),
            BLACK_METAMEMBER(artifactsXSwiftBus),
            BLACK_METAMEMBER(distributions)
        );
    };

    //! Trait for update info, i.e. distributions and artifacts
    struct TUpdateInfo : public TDataTrait<CUpdateInfo>
    {
        //! Key in data cache
        static const char *key() { return "updateinfo"; }

        //! First load is synchronous
        static constexpr bool isPinned() { return true; }
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Db::CUpdateInfo)

#endif // guard
