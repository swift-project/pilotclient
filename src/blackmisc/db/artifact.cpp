// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/db/artifact.h"
#include "blackmisc/stringutils.h"
#include "config/buildconfig.h"
#include <QRegularExpression>
#include <QStringBuilder>

using namespace swift::config;
using namespace BlackMisc::Network;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Db, CArtifact)

namespace BlackMisc::Db
{
    CArtifact::CArtifact()
    {}

    CArtifact::CArtifact(
        const QString &name, const QString &version, const QString &md5,
        CArtifact::ArtifactType type, int size, bool existing, const CPlatform &platform) : m_name(name.trimmed()),
                                                                                            m_md5(md5), m_type(static_cast<int>(type)), m_size(size), m_existing(existing),
                                                                                            m_platform(platform)
    {
        this->setVersion(trimVersionString(version));
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
        if (m_size < 0) { return {}; }
        return CFileUtils::humanReadableFileSize(m_size);
    }

    bool CArtifact::matchesAnyPlatform(const CPlatform &platform) const
    {
        return m_platform.matchesAny(platform);
    }

    bool CArtifact::hasUnrestrictedDistribution() const
    {
        return m_distributions.containsUnrestricted();
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
        const CUrl url = d.getDownloadUrl();
        if (url.isEmpty()) { return CRemoteFile(); }
        rf.setUtcTimestamp(this->getUtcTimestamp());
        rf.setUrl(url);
        rf.setDescription(this->getPlatform().toQString() + " " + d.getChannel());
        return rf;
    }

    bool CArtifact::isNewerThanCurrentBuild() const
    {
        if (this->isUnknown()) { return false; }
        if (!this->hasVersion()) { return false; }
        return this->getQVersion() > CBuildConfig::getVersion();
    }

    QString CArtifact::convertToQString(bool i18n) const
    {
        return this->convertToQString(", ", i18n);
    }

    QString CArtifact::convertToQString(const QString &separator, bool i18n) const
    {
        Q_UNUSED(i18n);
        return u"name: " %
               this->getName() %
               separator %
               u"size: " %
               this->getFileSizeHumanReadable() %
               separator %
               u"OS: " %
               this->getPlatform().toQString(i18n) %
               separator %
               u"timestamp: " %
               this->getFormattedUtcTimestampYmdhms();
    }

    QVariant CArtifact::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexName: return QVariant::fromValue(m_name);
        case IndexMd5: return QVariant::fromValue(m_md5);
        case IndexPlatform: return m_platform.propertyByIndex(index.copyFrontRemoved());
        case IndexType: return QVariant::fromValue(m_type);
        case IndexSize: return QVariant::fromValue(m_size);
        case IndexSizeHumanReadable: return QVariant::fromValue(this->getFileSizeHumanReadable());
        case IndexVersionString: return QVariant::fromValue(m_version);
        case IndexQVersion: return QVariant::fromValue(this->getQVersion());
        case IndexDistributions: return QVariant::fromValue(m_distributions);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CArtifact::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CArtifact>();
            return;
        }
        if (IDatastoreObjectWithIntegerKey::canHandleIndex(index))
        {
            IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant);
            return;
        }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexName: this->setName(variant.toString()); break;
        case IndexMd5: m_md5 = variant.toString(); break;
        case IndexPlatform: m_platform.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexType: m_type = variant.toInt(); break;
        case IndexSize: m_size = variant.toInt(); break;
        case IndexVersionString: m_version = variant.toString(); break;
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
        artifact.setKeyVersionTimestampFromDatabaseJson(json);
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
        static const QString xswb("xswiftbus");
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

    CPlatform CArtifact::artifactNameToPlatform(const QString &name)
    {
        if (name.isEmpty()) { return CPlatform::unknownOs(); }
        const QString n(name.toLower().trimmed());
        if (n.contains("-windows-") || n.endsWith(".exe"))
        {
            if (n.contains("-64-")) { return CPlatform::win64Platform(); }
            if (n.contains("-32-")) { return CPlatform::win32Platform(); }
            return CPlatform::unknownOs();
        }

        if (n.contains("-macos-") || n.endsWith(".dmg")) { return CPlatform::macOSPlatform(); }
        if (n.contains("-linux-") || n.endsWith(".run")) { return CPlatform::linuxPlatform(); }
        if (n.contains("-allos-")) { return CPlatform::allOs(); }

        return CPlatform::unknownOs();
    }

    QString CArtifact::versionNumberFromFilename(const QString &filename)
    {
        if (filename.isEmpty()) { return {}; }

        // swiftinstaller-linux-64-0.9.2.123.run
        thread_local const QRegularExpression regex { R"(\d+\.\d+\.\d+\.\d+)" };
        const QRegularExpressionMatch match = regex.match(filename);
        return match.captured();
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
        // old schema: yMMddHHmm (9)
        if (seg.length() >= 9) { return QStringLiteral("0"); }
        return seg;
    }
} // ns
