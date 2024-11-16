// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DB_UPDATEINFO_H
#define SWIFT_MISC_DB_UPDATEINFO_H

#include <QStringList>

#include "misc/datacache.h"
#include "misc/db/artifactlist.h"
#include "misc/db/distributionlist.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::db, CUpdateInfo)

namespace swift::misc::db
{
    //! Update info, i.e. artifacts and distributions
    //! \sa CArtifact
    //! \sa CDistribution
    class SWIFT_MISC_EXPORT CUpdateInfo : public CValueObject<CUpdateInfo>
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

        //! Artifacts (xswiftbus)
        const CArtifactList &getArtifactsXSwiftBus() const { return m_artifactsXSwiftBus; }

        //! Artifacts (xswiftbus)
        CArtifactList getArtifactsXSwiftBusLatestVersionFirst() const;

        //! Artifacts (xswiftbus)
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

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! To string
        QString convertToQString(const QString &separator, bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! Object from database JSON format
        static CUpdateInfo fromDatabaseJson(const QJsonObject &json, const QString &prefix = {});

        //! Object from database JSOn format
        static CUpdateInfo fromDatabaseJson(const QString &jsonString);

        //! Object from GitHub Releases REST API JSON format
        static CUpdateInfo fromGitHubReleasesJson(const QByteArray &jsonData);

    private:
        CArtifactList m_artifactsPilotClient; //!< artifacts pilot client
        CArtifactList m_artifactsXSwiftBus; //!< artifacts xswiftbus
        CDistributionList m_distributions; //!< all distributions (for any artifacts)

        SWIFT_METACLASS(
            CUpdateInfo,
            SWIFT_METAMEMBER(artifactsPilotClient),
            SWIFT_METAMEMBER(artifactsXSwiftBus),
            SWIFT_METAMEMBER(distributions));
    };

    //! Trait for update info, i.e. distributions and artifacts
    struct TUpdateInfo : public TDataTrait<CUpdateInfo>
    {
        //! Key in data cache
        static const char *key() { return "updateinfo"; }

        //! First load is synchronous
        static constexpr bool isPinned() { return true; }
    };
} // namespace swift::misc::db

Q_DECLARE_METATYPE(swift::misc::db::CUpdateInfo)

#endif // guard
