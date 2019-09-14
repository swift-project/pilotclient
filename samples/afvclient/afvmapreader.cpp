#include "afvmapreader.h"
#include "blackcore/application.h"
#include "dto.h"
#include <QEventLoop>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

AFVMapReader::AFVMapReader(QObject *parent) : QObject(parent)
{
    model = new AtcStationModel(this);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AFVMapReader::updateFromMap);
    timer->start(3000);
}

void AFVMapReader::updateFromMap()
{
    if (! sApp) { return; }

    QEventLoop loop;
    connect(sApp->getNetworkAccessManager(), &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    QNetworkReply *reply = sApp->getNetworkAccessManager()->get(QNetworkRequest(QUrl("https://afv-map.vatsim.net/atis-map-data")));
    while (! reply->isFinished()) { loop.exec(); }
    QByteArray jsonData = reply->readAll();
    reply->deleteLater();

    if (jsonData.isEmpty()) { return; }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isObject())
    {
        QJsonObject rootObject = jsonDoc.object();
        QVector<AtcStation> transceivers;

        if (rootObject.contains("controllers"))
        {
            QJsonObject otherObject = rootObject.value("controllers").toObject();
            for (auto it = otherObject.begin(); it != otherObject.end(); ++it)
            {
                QString callsign = it.key();
                if (it.value().isObject())
                {
                    QJsonObject stationObject = it.value().toObject();
                    if (stationObject.contains("transceivers"))
                    {
                        QJsonArray txArray = stationObject.value("transceivers").toArray();
                        for (auto jt = txArray.begin(); jt != txArray.end(); ++jt)
                        {
                            TransceiverDto transceiver = TransceiverDto::fromJson(jt->toObject());
                            transceivers.push_back( { callsign, transceiver} );
                        }
                    }
                }
            }
        }

        if (rootObject.contains("other") && rootObject.value("other").isObject())
        {
            QJsonObject otherObject = rootObject.value("other").toObject();
            for (auto it = otherObject.begin(); it != otherObject.end(); ++it)
            {
                QString callsign = it.key();
                if (it.value().isObject())
                {
                    QJsonObject stationObject = it.value().toObject();
                    if (stationObject.contains("transceivers"))
                    {
                        QJsonArray txArray = stationObject.value("transceivers").toArray();
                        for (auto jt = txArray.begin(); jt != txArray.end(); ++jt)
                        {
                            TransceiverDto transceiver = TransceiverDto::fromJson(jt->toObject());
                            transceivers.push_back( { callsign, transceiver} );
                        }
                    }
                }
            }
        }

        if (transceivers.isEmpty()) { return; }
        transceivers.erase(std::remove_if(transceivers.begin(), transceivers.end(), [this](const AtcStation &s)
        {
            return s.callsign() == m_callsign;
        }),
        transceivers.end());
        model->updateAtcStations(transceivers);
    }
}
