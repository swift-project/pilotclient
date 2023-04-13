/* Copyright (C) 2020
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackcore/githubpackagesreader.h"
#include "blackcore/application.h"
#include "blackconfig/buildconfig.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringBuilder>
#include <QUrl>

using namespace BlackMisc::Db;
using namespace BlackConfig;

namespace BlackCore
{
    CGitHubPackagesReader::CGitHubPackagesReader(QObject *parent) : QObject(parent)
    {}

    void CGitHubPackagesReader::readUpdateInfo()
    {
        // https://docs.github.com/en/rest/reference/repos#releases

        const QNetworkRequest request(QUrl(CBuildConfig::gitHubRepoApiUrl() % u"releases"));
        auto reply = sApp->getNetworkAccessManager()->get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply] {
            if (reply->error() == QNetworkReply::NoError)
            {
                const auto updateInfo = CUpdateInfo::fromGitHubReleasesJson(reply->readAll());
                if (!updateInfo.isEmpty())
                {
                    m_updateInfo.set(updateInfo);
                }
            }
            reply->deleteLater();
        });
    }

    CUpdateInfo CGitHubPackagesReader::getUpdateInfo() const
    {
        return m_updateInfo.get();
    }
}
