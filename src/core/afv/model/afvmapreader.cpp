// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/afv/model/afvmapreader.h"

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

#include "core/afv/dto.h"
#include "core/application.h"
#include "misc/aviation/callsign.h"

using namespace swift::misc::aviation;

namespace swift::core::afv::model
{
    CAfvMapReader::CAfvMapReader(QObject *parent) : QObject(parent)
    {
        m_model = new CSampleAtcStationModel(this);
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &CAfvMapReader::updateFromMap);
        m_timer->start(3000);
    }

    void CAfvMapReader::updateFromMap()
    {
        if (!sApp || !sApp->getNetworkAccessManager() || sApp->isShuttingDown()) { return; }

        QEventLoop loop(sApp);
        connect(sApp->getNetworkAccessManager(), &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
        connect(sApp, &CApplication::aboutToShutdown, &loop, &QEventLoop::quit);
        const QUrl url =
            sApp->getGlobalSetup().getAfvApiServerUrl().withAppendedPath("/api/v1/network/online/callsigns");
        QNetworkReply *reply = sApp->getNetworkAccessManager()->get(QNetworkRequest(url));
        while (reply && !reply->isFinished() && sApp && !sApp->isShuttingDown()) { loop.exec(); }
        const QByteArray jsonData = reply ? reply->readAll() : QByteArray {};
        if (reply) { reply->deleteLater(); }

        if (jsonData.isEmpty()) { return; }

        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
        if (jsonDoc.isArray())
        {
            const QJsonArray rootArray = jsonDoc.array();
            QVector<CSampleAtcStation> transceivers;
            QString callsign;

            for (auto it : rootArray)
            {
                if (it.isObject())
                {
                    const QJsonObject stationObject = it.toObject();

                    if (stationObject.contains("callsign")) { callsign = stationObject.value("callsign").toString(); }

                    if (callsign.isEmpty() || !CCallsign::looksLikeAtcCallsign(callsign)) { continue; }

                    if (stationObject.contains("transceivers"))
                    {
                        QJsonArray txArray = stationObject.value("transceivers").toArray();
                        for (auto &&jt : txArray)
                        {
                            const TransceiverDto transceiver = TransceiverDto::fromJson(jt.toObject());
                            transceivers.push_back({ callsign, transceiver });
                        }
                    }
                }
            }

            if (transceivers.isEmpty()) { return; }
            transceivers.erase(
                std::remove_if(transceivers.begin(), transceivers.end(),
                               [this](const CSampleAtcStation &s) { return s.callsign() == m_callsign; }),
                transceivers.end());
            m_model->updateAtcStations(transceivers);
        }
    }
} // namespace swift::core::afv::model
