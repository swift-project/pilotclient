/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "distribution.h"
#include "blackconfig/buildconfig.h"
#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Db
    {
        CDistribution::CDistribution()
        { }

        CDistribution::CDistribution(const QString &channel, bool restricted) :
            m_channel(channel.trimmed().toUpper()), m_restricted(restricted)
        { }

        void CDistribution::setChannel(const QString &channel)
        {
            m_channel = channel.trimmed().toUpper();
        }

        QStringList CDistribution::getPlatforms() const
        {
            QStringList platforms = m_platformFiles.keys();
            platforms.sort();
            return platforms;
        }

        QString CDistribution::guessPlatform() const
        {
            const QStringList platforms(getPlatforms());
            if (platforms.isEmpty()) { return ""; }
            QStringList reduced;
            for (const QString &p : platforms)
            {
                if (CBuildConfig::isRunningOnWindowsNtPlatform())
                {
                    if (!p.contains("win", Qt::CaseInsensitive)) continue;
                }
                else if (CBuildConfig::isRunningOnLinuxPlatform())
                {
                    if (!p.contains("linux", Qt::CaseInsensitive)) continue;
                }
                else if (CBuildConfig::isRunningOnMacOSXPlatform())
                {
                    if (!p.contains("mac", Qt::CaseInsensitive) || !p.contains("osx", Qt::CaseInsensitive)) continue;
                }
                reduced << p;
            }

            if (reduced.size() > 1)
            {
                // further reduce by VATSIM flag
                QStringList furtherReduced;
                for (const QString &p : reduced)
                {
                    if (CBuildConfig::isVatsimVersion())
                    {
                        if (p.contains("vatsim")) { furtherReduced << p; }
                    }
                    else
                    {
                        if (!p.contains("vatsim")) { furtherReduced << p; }
                    }
                }
                if (!furtherReduced.isEmpty()) { reduced = furtherReduced; }
            }

            if (reduced.isEmpty()) { return ""; }
            return reduced.front();
        }

        QString CDistribution::getVersionString(const QString &platform) const
        {
            if (platform.isEmpty()) { return ""; }
            return m_platformVersions.value(platform);
        }

        QVersionNumber CDistribution::getQVersion(const QString &platform) const
        {
            return QVersionNumber::fromString(getVersionString(platform));
        }

        QString CDistribution::getFilename(const QString &platform) const
        {
            if (platform.isEmpty()) { return ""; }
            return m_platformFiles.value(platform);
        }

        void CDistribution::addDownloadUrl(const CUrl &url)
        {
            if (url.isEmpty()) { return; }
            this->m_downloadUrls.push_back(url);
        }

        bool CDistribution::hasDownloadUrls() const
        {
            return !this->m_downloadUrls.isEmpty();
        }

        QString CDistribution::convertToQString(bool i18n) const
        {
            return convertToQString(", ", i18n);
        }

        QString CDistribution::convertToQString(const QString &separator, bool i18n) const
        {
            return QLatin1String("channel: ") %
                   this->getChannel() %
                   separator %
                   QLatin1String("download URLs: ") %
                   getDownloadUrls().toQString(i18n) %
                   separator %
                   QLatin1String("platforms: ") %
                   getPlatforms().join(", ") %
                   separator %
                   QLatin1String("timestamp: ") %
                   this->getFormattedUtcTimestampYmdhms();
        }

        CVariant CDistribution::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexChannel:
                return CVariant::fromValue(this->m_channel);
            case IndexDownloadUrls:
                return CVariant::fromValue(this->m_downloadUrls);
            case IndexRestricted:
                return CVariant::fromValue(this->m_restricted);
            case IndexPlatforms:
                return CVariant::fromValue(this->getPlatforms());
            case IndexPlatformFiles:
                return CVariant::fromValue(this->m_platformFiles);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CDistribution::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CDistribution>(); return; }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index))
            {
                IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant);
                return;
            }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexChannel:
                this->setChannel(variant.value<QString>());
                break;
            case IndexDownloadUrls:
                this->m_downloadUrls = variant.value<CUrlList>();
                break;
            case IndexRestricted:
                this->m_restricted = variant.toBool();
                break;
            case IndexPlatformFiles:
                this->m_platformFiles = variant.value<CPlatformDictionary>();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        CDistribution CDistribution::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            Q_UNUSED(prefix); // not nested

            const QString channel(json.value("channel").toString());
            const bool restricted(json.value("restricted").toBool());
            CDistribution distribution(channel, restricted);

            // add the URLs
            for (int i = 0; i < 5; i++)
            {
                const QString key = "url" + QString::number(i);
                const QString url = json.value(key).toString();
                if (url.isEmpty()) { continue; }
                distribution.addDownloadUrl(CUrl(url));
            }

            const QJsonObject platforms = json.value("platforms").toObject();
            const QStringList platformsKeys = platforms.keys();
            if (platformsKeys.isEmpty()) { return CDistribution(); } // no platforms, then the whole distribution is useless
            for (const QString platformKey : platformsKeys)
            {
                QStringList platformFileNames;
                QJsonArray platformFiles = platforms.value(platformKey).toArray();
                for (const QJsonValue &platformFile : platformFiles)
                {
                    const QString filename = platformFile.toString();
                    if (filename.isEmpty()) { continue; }
                    platformFileNames << filename;
                }

                const QPair<QString, QVersionNumber> latestFileInfo = findLatestVersion(platformFileNames);
                if (!latestFileInfo.first.isEmpty())
                {
                    distribution.m_platformFiles.insert(platformKey, latestFileInfo.first);
                    distribution.m_platformVersions.insert(platformKey, latestFileInfo.second.toString());
                }
            }

            distribution.setKeyAndTimestampFromDatabaseJson(json);
            return distribution;
        }

        QVersionNumber CDistribution::versionNumberFromFilename(const QString &filename)
        {
            if (filename.isEmpty()) { return QVersionNumber(); }

            // swift-installer-linux-64-0.7.3_2017-03-25_11-24-50.run
            static const QRegExp firstSegments("\\d+\\.\\d+\\.\\d+");
            if (firstSegments.indexIn(filename) < 0)
            {
                return QVersionNumber(); // no version, invalid
            }
            QString v = firstSegments.cap(0); // first 3 segments, like 0.9.3
            if (!v.endsWith('.')) { v += '.'; }

            static const QRegExp ts1("\\d{4}.\\d{2}.\\d{2}.\\d{2}.\\d{2}.\\d{2}");
            if (ts1.indexIn(filename) < 0)
            {
                // version without timestamp
                v += "0";
                return QVersionNumber::fromString(v);
            }

            const QString versionTimestampString = BlackMisc::digitOnlyString(ts1.cap(0));
            const QDateTime versionTimestamp = QDateTime::fromString(versionTimestampString, "yyyyMMddHHmmss");
            const QString lastSegment = QString::number(CBuildConfig::buildTimestampAsVersionSegment(versionTimestamp));

            v += lastSegment;
            return QVersionNumber::fromString(v);
        }

        QPair<QString, QVersionNumber> CDistribution::findLatestVersion(const QStringList &filenames)
        {
            if (filenames.isEmpty()) return QPair<QString, QVersionNumber>();
            if (filenames.size() == 1) return QPair<QString, QVersionNumber>(filenames.first(), versionNumberFromFilename(filenames.first()));
            QString latest;
            QVersionNumber latestVersion;
            for (const QString &fn : filenames)
            {
                if (latest.isEmpty())
                {
                    latest = fn;
                    latestVersion = versionNumberFromFilename(fn);
                    continue;
                }

                const QVersionNumber version = versionNumberFromFilename(fn);
                if (version > latestVersion)
                {
                    latest = fn;
                    latestVersion = versionNumberFromFilename(fn);
                }
            }
            return QPair<QString, QVersionNumber>(latest, latestVersion);
        }
    } // ns
} // ns
