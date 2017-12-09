/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "artifact.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"
#include <QRegularExpression>
#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Db
    {
        CArtifact::CArtifact()
        { }

        CArtifact::CArtifact(
            const QString &name, const QString &version, const QString &md5,
            CArtifact::ArtifactType type, int size, bool existing, const CPlatform &platform):
            m_name(name.trimmed()), m_version(trimVersionString(version)),
            m_md5(md5), m_type(static_cast<int>(type)), m_size(size), m_existing(existing),
            m_platform(platform)
        {
            if (!name.isEmpty() && version.isEmpty())
            {
                m_version = versionNumberFromFilename(name);
            }
        }

        bool CArtifact::matchesName(const QString &name, Qt::CaseSensitivity cs) const
        {
            const bool m = (cs == Qt::CaseInsensitive) ?
                           caseInsensitiveStringCompare(this->getName(), name) :
                           name == this->getName();
            if (m) { return true; }
            return name.startsWith(this->getName(), cs);
        }

        QString CArtifact::getFileSizeHumanReadable() const
        {
            if (m_size < 0) { return QStringLiteral(""); }
            return CFileUtils::humanReadableFileSize(m_size);
        }

        bool CArtifact::matchesAnyPlatform(const CPlatform &platform) const
        {
            return m_platform.matchesAny(platform);
        }

        bool CArtifact::isWithDistribution(const CDistribution &distribution, bool acceptMoreStableDistributions) const
        {
            if (distribution.isEmpty() || !this->hasDistributions()) { return false; }
            for (const CDistribution &dist : this->getDistributions())
            {
                if (dist == distribution) { return true; }
                if (acceptMoreStableDistributions && dist.isStabilityBetter(distribution)) { return true; }
            }
            return false;
        }

        CRemoteFile CArtifact::asRemoteFile() const
        {
            if (!this->hasDistributions()) { return CRemoteFile(); }
            CRemoteFile rf(this->getName(), this->getFileSize());
            const CDistribution d = this->getMostStableDistribution();
            const CUrl url = d.getDownloadUrls().getRandomUrl();
            rf.setUtcTimestamp(this->getUtcTimestamp());
            rf.setUrl(url);
            rf.setDescription(this->getPlatform().toQString() + " " + d.getChannel());
            return rf;
        }

        QVersionNumber CArtifact::getQVersion() const
        {
            return QVersionNumber::fromString(getVersionString());
        }

        bool CArtifact::isNewerThanCurrentBuild() const
        {
            if (this->isUnknown()) { return false; }
            if (this->getVersionString().isEmpty()) { return false; }
            return this->getQVersion() > CBuildConfig::getVersion();
        }

        QString CArtifact::convertToQString(bool i18n) const
        {
            return this->convertToQString(", ", i18n);
        }

        QString CArtifact::convertToQString(const QString &separator, bool i18n) const
        {
            Q_UNUSED(i18n);
            return QLatin1String("name: ") %
                   this->getName() %
                   separator %
                   QLatin1String("size: ") %
                   this->getFileSizeHumanReadable() %
                   separator %
                   QLatin1String("OS: ") %
                   this->getPlatform().toQString(i18n) %
                   separator %
                   QLatin1String("timestamp: ") %
                   this->getFormattedUtcTimestampYmdhms();
        }

        CVariant CArtifact::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexName: return CVariant::fromValue(m_name);
            case IndexMd5: return CVariant::fromValue(m_md5);
            case IndexPlatform: return m_platform.propertyByIndex(index.copyFrontRemoved());
            case IndexType: return CVariant::fromValue(m_type);
            case IndexSize: return CVariant::fromValue(m_size);
            case IndexSizeHumanReadable: return CVariant::fromValue(this->getFileSizeHumanReadable());
            case IndexVersionString: return CVariant::fromValue(m_version);
            case IndexQVersion: return CVariant::fromValue(this->getQVersion());
            case IndexDistributions: return CVariant::fromValue(m_distributions);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CArtifact::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CArtifact>(); return; }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index))
            {
                IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant);
                return;
            }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexName: this->setName(variant.toQString()); break;
            case IndexMd5: m_md5 = variant.toQString(); break;
            case IndexPlatform: m_platform.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexType: m_type = variant.toInt(); break;
            case IndexSize: m_size = variant.toInt(); break;
            case IndexVersionString: m_version = variant.toQString(); break;
            case IndexDistributions: m_distributions = variant.value<CDistributionList>(); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        CArtifact CArtifact::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            Q_UNUSED(prefix); // not nested

            const QString name = json.value("name").toString();
            const QString md5 = json.value("md5").toString();
            const QString version = json.value("version").toString();
            const CPlatform platform = CPlatform(json.value("os").toString());
            const CArtifact::ArtifactType type = stringToType(json.value("type").toString());
            const int size = json.value("size").toInt(-1);
            const bool existing = json.value("existing").toBool();

            CArtifact artifact(name, version, md5, type, size, existing, platform);
            artifact.setKeyAndTimestampFromDatabaseJson(json);
            if (json.contains("distributions"))
            {
                const QJsonObject distJson = json.value("distributions").toObject();
                if (!distJson.isEmpty() && distJson.contains("distributionArray"))
                {
                    const CDistributionList distributions = CDistributionList::fromDatabaseJson(distJson.value("distributionArray").toArray());
                    artifact.setDistributions(distributions);
                }
            }
            return artifact;
        }

        const QString &CArtifact::typeToString(CArtifact::ArtifactType type)
        {
            static const QString xswb("xSwiftBus");
            static const QString installer("pilot client installer");
            static const QString symbols("symbols");
            static const QString unknown("unknown");

            switch (type)
            {
            case XSwiftBus: return xswb;
            case PilotClientInstaller: return installer;
            case Symbols: return symbols;
            case UnknownArtifact:
            default: break;
            }
            return unknown;
        }

        QString CArtifact::versionNumberFromFilename(const QString &filename)
        {
            if (filename.isEmpty()) { return QStringLiteral(""); }

            // swift-installer-linux-64-0.7.3_2017-03-25_11-24-50.run
            thread_local const QRegularExpression firstSegments("\\d+\\.\\d+\\.\\d+");
            const QRegularExpressionMatch firstSegmentsMatch = firstSegments.match(filename);
            if (!firstSegmentsMatch.hasMatch())
            {
                return QStringLiteral(""); // no version, invalid
            }
            QString v = firstSegmentsMatch.captured(0); // first 3 segments, like 0.9.3
            if (!v.endsWith('.')) { v += '.'; }

            thread_local const QRegularExpression ts1("\\d{4}.?\\d{2}.?\\d{2}.?\\d{2}.?\\d{2}.?\\d{2}");
            const QRegularExpressionMatch ts1Match = ts1.match(filename);
            if (!ts1Match.hasMatch())
            {
                // version without timestamp
                v += "0";
                return v;
            }

            const QString versionTimestampString = BlackMisc::digitOnlyString(ts1Match.captured(0));
            const QDateTime versionTimestamp = QDateTime::fromString(versionTimestampString, "yyyyMMddHHmmss");
            const QString lastSegment = QString::number(CBuildConfig::buildTimestampAsVersionSegment(versionTimestamp));

            v += lastSegment;
            return v;
        }

        CArtifact::ArtifactType CArtifact::stringToType(const QString &str)
        {
            const QString s(str.trimmed().toLower());
            if (s.contains("installer")) return CArtifact::PilotClientInstaller;
            if (s.contains("client")) return CArtifact::PilotClientInstaller;
            if (s.contains("symb")) return CArtifact::Symbols;
            if (s.contains("bus")) return CArtifact::XSwiftBus;
            return CArtifact::UnknownArtifact;
        }

        QString CArtifact::trimVersionString(const QString &version)
        {
            if (version.count('.') != 3) return version; // no 4th segment
            QStringList parts = version.split('.');
            const QString p4 = trim4thSegment(parts[3]);
            if (p4 == parts[3]) { return version; } // nothing changed
            parts[3] = p4;
            const QString v = parts.join('.');
            return v;
        }

        QString CArtifact::trim4thSegment(const QString &seg)
        {
            // yyyyMMddHHmmss (14): offset is 2010xxxxx
            if (seg.length() <= 13) { return seg; }
            const int fs = CBuildConfig::buildTimestampAsVersionSegment(QDateTime::fromString(seg, "yyyyMMddHHmmss"));
            return QString::number(fs);
        }
    } // ns
} // ns
