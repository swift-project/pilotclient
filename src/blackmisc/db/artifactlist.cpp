/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "artifactlist.h"
#include "blackmisc/stringutils.h"

using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Db
    {
        CArtifactList::CArtifactList() { }

        CArtifactList::CArtifactList(const CSequence<CArtifact> &other) :
            CSequence<CArtifact>(other)
        { }

        CPlatformSet CArtifactList::getPlatforms() const
        {
            CPlatformSet platforms;
            for (const CArtifact &artifact : *this)
            {
                platforms.insert(artifact.getPlatform());
            }
            return platforms;
        }

        CArtifact CArtifactList::findFirstByMatchingPlatformOrDefault(const CPlatform &platform) const
        {
            return this->findFirstByOrDefault(&CArtifact::getPlatform, platform.getPlatform());
        }

        CArtifact CArtifactList::findFirstByMatchingNameOrDefault(const QString &name, Qt::CaseSensitivity cs) const
        {
            for (const CArtifact &artifact : *this)
            {
                if (artifact.matchesName(name, cs)) { return artifact; }
            }
            return CArtifact();
        }

        CArtifactList CArtifactList::findByMatchingPlatform(const CPlatform &platform) const
        {
            CArtifactList artifacts;
            for (const CArtifact &artifact : *this)
            {
                if (artifact.matchesAnyPlatform(platform)) { artifacts.push_back(artifact); }
            }
            return artifacts;
        }

        CArtifactList CArtifactList::findByType(const CArtifact::ArtifactType type) const
        {
            return this->findBy(&CArtifact::getType, type);
        }

        CArtifactList CArtifactList::findMatchingForCurrentPlatform() const
        {
            return this->findByMatchingPlatform(CPlatform::currentPlatform());
        }

        CArtifactList CArtifactList::findByDistribution(const CDistribution &distribution, bool findMoreStableDistribution) const
        {
            CArtifactList al;
            for (const CArtifact &artifact : *this)
            {
                if (!artifact.isWithDistribution(distribution, findMoreStableDistribution)) { continue; }
                al.push_back(artifact);
            }
            return al;
        }

        CArtifactList CArtifactList::findWithUnrestrictedDistributions() const
        {
            CArtifactList al;
            for (const CArtifact &artifact : *this)
            {
                if (!artifact.hasUnrestrictedDistribution()) { continue; }
                al.push_back(artifact);
            }
            return al;
        }

        CArtifactList CArtifactList::findByDistributionAndPlatform(const CDistribution &distribution, const CPlatform &platform, bool findMoreStableDistributions) const
        {
            return this->findByMatchingPlatform(platform).findByDistribution(distribution, findMoreStableDistributions);
        }

        CArtifact CArtifactList::findFirstByVersionOrDefault(const QVersionNumber &version) const
        {
            return this->findFirstByOrDefault(&CArtifact::getQVersion, version);
        }

        CArtifact CArtifactList::findFirstByVersionOrDefault(const QString &version) const
        {
            const QVersionNumber v = QVersionNumber::fromString(version);
            return this->findFirstByVersionOrDefault(v);
        }

        CDistributionList CArtifactList::getDistributions() const
        {
            CDistributionList dl;
            for (const CArtifact &artifact : *this)
            {
                for (const CDistribution &d : artifact.getDistributions())
                {
                    // add only once
                    if (dl.contains(d)) { continue; }
                    dl.push_back(d);
                }
            }
            return dl;
        }

        void CArtifactList::sortByVersion(Qt::SortOrder order)
        {
            this->sort([order](const CArtifact & a, const CArtifact & b)
            {
                const QVersionNumber av = a.getQVersion();
                const QVersionNumber bv = b.getQVersion();
                return order == Qt::AscendingOrder ? av < bv : bv < av;
            });
        }

        CArtifact CArtifactList::getLatestArtifactOrDefault() const
        {
            if (this->size() < 2) { return this->frontOrDefault(); }
            CArtifactList copy(*this);
            copy.sortByVersion();
            return copy.back();
        }

        CRemoteFileList CArtifactList::asRemoteFiles() const
        {
            CRemoteFileList rfs;
            for (const CArtifact &artifact : *this)
            {
                const CRemoteFile rf = artifact.asRemoteFile();
                if (rf.hasName())
                {
                    rfs.push_back(rf);
                }
            }
            return rfs;
        }

        QString CArtifactList::getLatestVersion() const
        {
            return this->getLatestArtifactOrDefault().getVersion();
        }

        QVersionNumber CArtifactList::getLatestQVersion() const
        {
            return this->getLatestArtifactOrDefault().getQVersion();
        }

        QStringList CArtifactList::getSortedNames() const
        {
            CArtifactList copy(*this);
            copy.sortByVersion(Qt::DescendingOrder);
            QStringList names;
            for (const CArtifact &artifact : std::as_const(copy))
            {
                if (artifact.hasName())
                {
                    names.push_back(artifact.getName());
                }
            }
            return names;
        }

        QStringList CArtifactList::getSortedVersions() const
        {
            CArtifactList copy(*this);
            copy.sortByVersion(Qt::DescendingOrder);
            QStringList versions;
            for (const CArtifact &artifact : std::as_const(copy))
            {
                if (artifact.hasVersion())
                {
                    versions.push_back(artifact.getVersion());
                }
            }
            return versions;
        }

        qint64 CArtifactList::getTotalFileSize() const
        {
            qint64 s = 0;
            for (const CArtifact &a : *this)
            {
                s += a.getFileSize();
            }
            return s;
        }

        QString CArtifactList::getTotalFileSizeHumanReadable() const
        {
            return CFileUtils::humanReadableFileSize(this->getTotalFileSize());
        }

        CArtifactList CArtifactList::fromDatabaseJson(const QJsonArray &array)
        {
            CArtifactList artifacts;
            for (const QJsonValue &value : array)
            {
                const CArtifact artifact(CArtifact::fromDatabaseJson(value.toObject()));
                artifacts.push_back(artifact);
            }
            return artifacts;
        }

        CArtifactList CArtifactList::fromDatabaseJson(const QString &json)
        {
            if (json.isEmpty()) { return CArtifactList(); }
            return CArtifactList::fromDatabaseJson(Json::jsonArrayFromString(json));
        }
    } // namespace
} // namespace
