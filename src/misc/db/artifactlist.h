// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DB_ARTIFACTLIST_H
#define SWIFT_MISC_DB_ARTIFACTLIST_H

#include "misc/db/artifact.h"
#include "misc/db/distributionlist.h"
#include "misc/db/datastoreobjectlist.h"
#include "misc/network/remotefilelist.h"
#include "misc/platformset.h"
#include "misc/swiftmiscexport.h"
#include "misc/collection.h"
#include "misc/sequence.h"
#include <QSet>
#include <QString>

BLACK_DECLARE_SEQUENCE_MIXINS(swift::misc::db, CArtifact, CArtifactList)

namespace swift::misc::db
{
    //! Multiple artifacts
    class SWIFT_MISC_EXPORT CArtifactList :
        public CSequence<CArtifact>,
        public IDatastoreObjectList<CArtifact, CArtifactList, int>,
        public mixin::MetaType<CArtifactList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CArtifactList)
        using CSequence::CSequence;

        //! Empty constructor.
        CArtifactList();

        //! Construct from a base class object.
        CArtifactList(const CSequence<CArtifact> &other);

        //! All platforms for all channels
        CPlatformSet getPlatforms() const;

        //! Find first by platform
        CArtifact findFirstByMatchingPlatformOrDefault(const CPlatform &platform) const;

        //! FInd first by name
        CArtifact findFirstByMatchingNameOrDefault(const QString &name, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

        //! Find by platform
        CArtifactList findByMatchingPlatform(const CPlatform &platform) const;

        //! Find by type
        CArtifactList findByType(const CArtifact::ArtifactType type) const;

        //! Find for my platform
        CArtifactList findMatchingForCurrentPlatform() const;

        //! Find by distribution
        CArtifactList findByDistribution(const CDistribution &distribution, bool findMoreStableDistribution = false) const;

        //! Find artifacts with public (unrestricted) distributions
        CArtifactList findWithUnrestrictedDistributions() const;

        //! Find by distribution and platform
        CArtifactList findByDistributionAndPlatform(const CDistribution &distribution, const CPlatform &platform, bool findMoreStableDistributions = false) const;

        //! Find by version
        CArtifact findFirstByVersionOrDefault(const QVersionNumber &version) const;

        //! Find by version
        CArtifact findFirstByVersionOrDefault(const QString &version) const;

        //! All distributions related to these artifacts
        CDistributionList getDistributions() const;

        //! Sort by version
        void sortByVersion(Qt::SortOrder order = Qt::AscendingOrder);

        //! Latest (newest) artifact
        CArtifact getLatestArtifactOrDefault() const;

        //! As remote files
        network::CRemoteFileList asRemoteFiles() const;

        //! Latest version
        QString getLatestVersion() const;

        //! Latest version
        QVersionNumber getLatestQVersion() const;

        //! All unique names
        //! \note sorted by version, latest version first
        QStringList getSortedNames() const;

        //! All unique versions
        //! \note sorted by version, latest version first
        QStringList getSortedVersions() const;

        //! Size of all artifacts
        qint64 getTotalFileSize() const;

        //! Size of all artifacts
        QString getTotalFileSizeHumanReadable() const;

        //! From database JSON by array
        static CArtifactList fromDatabaseJson(const QJsonArray &array);

        //! From database JSON by string
        static CArtifactList fromDatabaseJson(const QString &json);
    };
} // ns

Q_DECLARE_METATYPE(swift::misc::db::CArtifactList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::db::CArtifact>)

#endif // guard
