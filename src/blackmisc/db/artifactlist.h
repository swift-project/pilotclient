/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DB_ARTIFACTLIST_H
#define BLACKMISC_DB_ARTIFACTLIST_H

#include "artifact.h"
#include "distributionlist.h"
#include "datastoreobjectlist.h"
#include "blackmisc/network/remotefilelist.h"
#include "blackmisc/platformset.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"
#include <QSet>
#include <QString>

namespace BlackMisc
{
    namespace Db
    {
        //! Multiple artifacts
        class BLACKMISC_EXPORT CArtifactList :
            public CSequence<CArtifact>,
            public IDatastoreObjectList<CArtifact, CArtifactList, int>,
            public Mixin::MetaType<CArtifactList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CArtifactList)
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
            Network::CRemoteFileList asRemoteFiles() const;

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
} // ns

Q_DECLARE_METATYPE(BlackMisc::Db::CArtifactList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Db::CArtifact>)

#endif //guard
