#include "blackmisc/sequence.h"
#include "blackmisc/avatcstation.h"
#include "blackmisc/nwuser.h"
#include "vatsimbookingreader.h"
#include <QtXml/QDomElement>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackCore
{

    CVatsimBookingReader::CVatsimBookingReader(const QString &url, QObject *parent) : QObject(parent), m_serviceUrl(url), m_networkManager(nullptr), m_updateTimer(nullptr)
    {
        this->m_networkManager = new QNetworkAccessManager(this);
        this->m_updateTimer = new QTimer(this);
        this->connect(this->m_networkManager, &QNetworkAccessManager::finished, this, &CVatsimBookingReader::loadFinished);
        this->connect(this->m_updateTimer, &QTimer::timeout, this, &CVatsimBookingReader::read);
    }

    void CVatsimBookingReader::read()
    {
        QUrl url(this->m_serviceUrl);
        if (url.isEmpty()) return;
        Q_ASSERT(this->m_networkManager);
        QNetworkRequest request(url);
        this->m_networkManager->get(request);
    }

    void CVatsimBookingReader::setInterval(int updatePeriodMs)
    {
        Q_ASSERT(this->m_updateTimer);
        if (updatePeriodMs < 1)
            this->m_updateTimer->stop();
        else
            this->m_updateTimer->start(updatePeriodMs);
    }

    /*
     * Bookings read from XML
     */
    void CVatsimBookingReader::loadFinished(QNetworkReply *nwReply)
    {
        if (nwReply->error() == QNetworkReply::NoError)
        {
            QString xmlData = nwReply->readAll();
            QDomDocument doc;

            if (doc.setContent(xmlData))
            {
                QDomNode atc = doc.elementsByTagName("atcs").at(0);
                QDomNodeList bookingNodes = atc.toElement().elementsByTagName("booking");
                int size = bookingNodes.size();
                CAtcStationList bookedStations;
                for (int i = 0; i < size; i++)
                {
                    QDomNode bookingNode = bookingNodes.at(i);
                    QDomNodeList bookingNodeValues = bookingNode.childNodes();
                    CAtcStation bookedStation;
                    CUser user;
                    for (int v = 0; v < bookingNodeValues.size(); v++)
                    {
                        QDomNode bookingNodeValue = bookingNodeValues.at(v);
                        QString name = bookingNodeValue.nodeName().toLower();
                        QString value = bookingNodeValue.toElement().text();
                        if (name == "id")
                        {
                            // could be used as unique key
                        }
                        else if (name == "callsign")
                        {
                            bookedStation.setCallsign(CCallsign(value));
                        }
                        else if (name == "name")
                        {
                            user.setRealName(value);
                        }
                        else if (name == "cid")
                        {
                            user.setId(value);
                        }
                        else if (name == "time_end")
                        {
                            QDateTime t = QDateTime::fromString(value, "yyyy-MM-dd HH:mm:ss");
                            bookedStation.setBookedUntilUtc(t);
                        }
                        else if (name == "time_start")
                        {
                            QDateTime t = QDateTime::fromString(value, "yyyy-MM-dd HH:mm:ss");
                            bookedStation.setBookedFromUtc(t);
                        }
                    }
                    // time checks
                    QDateTime now = QDateTime::currentDateTimeUtc();
                    if (now.msecsTo(bookedStation.getBookedUntilUtc()) < (1000 * 60 * 15)) continue; // until n mins in past
                    if (now.msecsTo(bookedStation.getBookedFromUtc()) > (1000 * 60 * 60 * 24)) continue; // to far in the future, n hours
                    bookedStation.setController(user);
                    bookedStations.push_back(bookedStation);
                }
                m_updateTimestamp = QDateTime::currentDateTimeUtc();
                emit this->bookingsRead(bookedStations);
                nwReply->close();
                nwReply->deleteLater();

            } // node
        } // content
    } // method
} // namespace
