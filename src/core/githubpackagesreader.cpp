// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "core/githubpackagesreader.h"
#include "core/application.h"
#include "config/buildconfig.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringBuilder>
#include <QUrl>

using namespace swift::misc::db;
using namespace swift::config;

namespace swift::core
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
