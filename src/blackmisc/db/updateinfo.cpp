/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/db/updateinfo.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"
#include <QStringBuilder>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

using namespace BlackConfig;

namespace BlackMisc::Db
{
    CUpdateInfo::CUpdateInfo(const CArtifactList &artifacts, const CDistributionList &distributions) :
        m_artifactsPilotClient(artifacts.findByType(CArtifact::PilotClientInstaller)),
        m_artifactsXSwiftBus(artifacts.findByType(CArtifact::XSwiftBus)),
        m_distributions(distributions)
    {
        // void
    }

    CDistributionList CUpdateInfo::getDistributionsPilotClientForCurrentPlatform() const
    {
        const CArtifactList artifacts = this->getArtifactsPilotClient().findMatchingForCurrentPlatform();
        CDistributionList distributions = artifacts.getDistributions();
        distributions.sortByStability();
        return distributions;
    }

    CArtifactList CUpdateInfo::getArtifactsPilotClientForCurrentPlatform() const
    {
        CArtifactList artifacts = m_artifactsPilotClient.findMatchingForCurrentPlatform();
        artifacts.sortByVersion(Qt::DescendingOrder);
        return artifacts;
    }

    CArtifactList CUpdateInfo::getArtifactsXSwiftBusLatestVersionFirst() const
    {
        CArtifactList artifacts(m_artifactsXSwiftBus);
        artifacts.sortByVersion(Qt::DescendingOrder);
        return artifacts;
    }

    CArtifactList CUpdateInfo::getArtifactsXSwiftBusOldestVersionFirst() const
    {
        CArtifactList artifacts(m_artifactsXSwiftBus);
        artifacts.sortByVersion(Qt::DescendingOrder);
        return artifacts;
    }

    CArtifactList CUpdateInfo::getArtifactsXSwiftBusForCurrentPlatform() const
    {
        CArtifactList artifacts = m_artifactsXSwiftBus.findMatchingForCurrentPlatform();
        artifacts.sortByVersion(Qt::DescendingOrder);
        return artifacts;
    }

    CDistribution CUpdateInfo::anticipateOwnDistribution() const
    {
        if (this->isEmpty()) { return CDistribution(); }
        const CArtifactList ownArtifacts = this->getArtifactsPilotClientForCurrentPlatform();
        if (ownArtifacts.isEmpty()) { return CDistribution(); }

        const QVersionNumber myVersion = CBuildConfig::getVersion();
        const QVersionNumber latestVersion = ownArtifacts.getLatestQVersion();
        if (myVersion > latestVersion)
        {
            // dev. version
            return ownArtifacts.getDistributions().getLeastStableOrDefault();
        }

        const CArtifact exactVersion = ownArtifacts.findFirstByVersionOrDefault(myVersion);
        if (!exactVersion.isUnknown()) { return exactVersion.getDistributions().getMostStableOrDefault(); }

        return CDistribution::localDeveloperBuild();
    }

    QStringList CUpdateInfo::anticipateMyDefaultChannelAndPlatform() const
    {
        const CArtifactList myArtifacts = this->getArtifactsPilotClient().findMatchingForCurrentPlatform();
        const CDistribution mostStable = myArtifacts.getDistributions().getMostStableOrDefault();
        return QStringList({ mostStable.getClassName(), CPlatform::currentPlatform().getPlatformName() });
    }

    QString CUpdateInfo::convertToQString(bool i18n) const
    {
        return this->convertToQString(", ", i18n);
    }

    QString CUpdateInfo::convertToQString(const QString &separator, bool i18n) const
    {
        Q_UNUSED(i18n);
        return u"artifacts (PC): " %
                this->getArtifactsPilotClient().toQString(i18n) %
                separator %
                u"artifacts (XSB): " %
                this->getArtifactsXSwiftBus().toQString(i18n) %
                separator %
                u"distributions: " %
                this->getDistributions().toQString(i18n);
    }

    QVariant CUpdateInfo::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexArtifactsPilotClient: return QVariant::fromValue(m_artifactsPilotClient);
        case IndexArtifactsXSwiftBus: return QVariant::fromValue(m_artifactsXSwiftBus);
        case IndexDistributions: return QVariant::fromValue(m_distributions);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CUpdateInfo::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CUpdateInfo>(); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexArtifactsPilotClient: m_artifactsPilotClient = variant.value<CArtifactList>(); break;
        case IndexArtifactsXSwiftBus: m_artifactsXSwiftBus = variant.value<CArtifactList>(); break;
        case IndexDistributions: m_distributions = variant.value<CDistributionList>(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    CUpdateInfo CUpdateInfo::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
    {
        Q_UNUSED(prefix); // not nested
        const QJsonArray jsonDistributions = json.value("distributions").toArray();
        const QJsonArray jsonArtifacts = json.value("artifacts").toArray();
        const CDistributionList distributions = CDistributionList::fromDatabaseJson(jsonDistributions);
        const CArtifactList artifacts = CArtifactList::fromDatabaseJson(jsonArtifacts);
        Q_ASSERT_X(jsonDistributions.size() == distributions.size(), Q_FUNC_INFO, "size mismatch");
        Q_ASSERT_X(artifacts.size() == artifacts.size(), Q_FUNC_INFO, "size mismatch");
        return CUpdateInfo(artifacts, distributions);
    }

    CUpdateInfo CUpdateInfo::fromDatabaseJson(const QString &jsonString)
    {
        if (jsonString.isEmpty()) { return CUpdateInfo(); }
        return CUpdateInfo::fromDatabaseJson(Json::jsonObjectFromString(jsonString));
    }

    CUpdateInfo CUpdateInfo::fromGitHubReleasesJson(const QByteArray &jsonData)
    {
        // https://docs.github.com/en/rest/reference/repos#releases

        const QString url = CBuildConfig::gitHubRepoUrl() + QStringLiteral("releases/download");
        CDistribution alphaDistribution("ALPHA", 5, false);
        CDistribution betaDistribution("BETA", 10, false);
        alphaDistribution.addDownloadUrl(url);
        betaDistribution.addDownloadUrl(url);

        CUpdateInfo result;
        result.m_distributions = { alphaDistribution, betaDistribution };

        for (const QJsonValue release : QJsonDocument::fromJson(jsonData).array())
        {
            QString version = release[QLatin1String("tag_name")].toString();
            if (version.isEmpty() || version[0] != 'v') { continue; }
            version.remove(0, 1);
            if (containsChar(version, [](QChar c) { return c != '.' && !is09(c); })) { continue; }

            bool existing = !release[QLatin1String("draft")].toBool();
            bool alpha = release[QLatin1String("prerelease")].toBool();

            for (const QJsonValue asset : release[QLatin1String("assets")].toArray())
            {
                QString name = asset[QLatin1String("name")].toString();
                QString filename = QStringLiteral("v%1/%2").arg(version, name);
                int size = asset[QLatin1String("size")].toInt();

                CArtifact::ArtifactType type = CArtifact::UnknownArtifact;
                if      (name.startsWith(QStringLiteral("swiftinstaller"))) { type = CArtifact::PilotClientInstaller; }
                else if (name.startsWith(QStringLiteral("swiftsymbols")))   { type = CArtifact::Symbols; }
                else if (name.startsWith(QStringLiteral("xswiftbus")))      { type = CArtifact::XSwiftBus; }

                CPlatform platform;
                if      (name.contains(QStringLiteral("windows-32"))) { platform = CPlatform::win32Platform(); }
                else if (name.contains(QStringLiteral("windows-64"))) { platform = CPlatform::win64Platform(); }
                else if (name.contains(QStringLiteral("linux-64")))   { platform = CPlatform::linuxPlatform(); }
                else if (name.contains(QStringLiteral("macos-64")))   { platform = CPlatform::macOSPlatform(); }
                else if (name.contains(QStringLiteral("allos")))      { platform = CPlatform::allOs(); }

                CArtifact artifact(filename, version, {}, type, size, existing, platform);
                artifact.setDistributions({ alpha ? alphaDistribution : betaDistribution });

                if (type == CArtifact::PilotClientInstaller) { result.m_artifactsPilotClient.push_back(artifact); }
                else if (type == CArtifact::XSwiftBus) { result.m_artifactsXSwiftBus.push_back(artifact); }
            }
        }
        return result;
    }
} // ns
