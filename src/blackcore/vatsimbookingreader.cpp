#include "blackmisc/sequence.h"
#include "blackmisc/avatcstation.h"
#include "blackmisc/nwuser.h"
#include "vatsimbookingreader.h"

#include <QtXml/QDomElement>
#include <QtConcurrent/QtConcurrent>

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
        QtConcurrent::run(this, &CVatsimBookingReader::parseBookings, nwReply);
    }

    /*
     * Parse bookings
     */
    void CVatsimBookingReader::parseBookings(QNetworkReply *nwReply)
    {
        if (nwReply->error() == QNetworkReply::NoError)
        {
            static const QString timestampFormat("yyyy-MM-dd HH:mm:ss");
            QString xmlData = nwReply->readAll();
            QDomDocument doc;

            if (doc.setContent(xmlData))
            {
                QDomNode timestamp = doc.elementsByTagName("timestamp").at(0);
                QString ts = timestamp.toElement().text().trimmed();
                Q_ASSERT(!ts.isEmpty());
                if (ts.isEmpty())
                {
                    // fallback
                    m_updateTimestamp = QDateTime::currentDateTimeUtc();
                }
                else
                {
                    // normally the timestamp is always updated from backend
                    // if this changes in the future we're prepared
                    QDateTime fileTimestamp = QDateTime::fromString(ts, timestampFormat);
                    fileTimestamp.setTimeSpec(Qt::UTC);
                    if (this->m_updateTimestamp == fileTimestamp) return; // nothing to do
                    this->m_updateTimestamp = fileTimestamp;
                }

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
                            QDateTime t = QDateTime::fromString(value, timestampFormat);
                            t.setTimeSpec(Qt::UTC);
                            bookedStation.setBookedUntilUtc(t);
                        }
                        else if (name == "time_start")
                        {
                            QDateTime t = QDateTime::fromString(value, timestampFormat);
                            t.setTimeSpec(Qt::UTC);
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
                emit this->dataRead(bookedStations);

            } // node
        } // content

        nwReply->close();
        nwReply->deleteLater();

    } // method
} // namespace
