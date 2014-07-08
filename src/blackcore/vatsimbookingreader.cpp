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
    CVatsimBookingReader::CVatsimBookingReader(const QString &url, QObject *parent) :
        QObject(parent), CThreadedReader(),
        m_serviceUrl(url), m_networkManager(nullptr)
    {
        this->m_networkManager = new QNetworkAccessManager(this);
        this->connect(this->m_networkManager, &QNetworkAccessManager::finished, this, &CVatsimBookingReader::ps_loadFinished);
        this->connect(this->m_updateTimer, &QTimer::timeout, this, &CVatsimBookingReader::read);
    }

    void CVatsimBookingReader::read()
    {
        QUrl url(this->m_serviceUrl);
        if (url.isEmpty()) return;
        Q_ASSERT(this->m_networkManager);
        QNetworkRequest request(url);
        QNetworkReply *reply = this->m_networkManager->get(request);
        this->setPendingNetworkReply(reply);
    }

    /*
     * Bookings read from XML
     */
    void CVatsimBookingReader::ps_loadFinished(QNetworkReply *nwReply)
    {
        this->setPendingNetworkReply(nullptr);
        if (!this->isStopped())
        {
            QFuture<void> f = QtConcurrent::run(this, &CVatsimBookingReader::parseBookings, nwReply);
            this->setPendingFuture(f);
        }
    }

    /*
     * Parse bookings
     */
    void CVatsimBookingReader::parseBookings(QNetworkReply *nwReply)
    {
        // Worker thread, make sure to write no members here!
        if (this->isStopped())
        {
            qDebug() << "terminated" << Q_FUNC_INFO;
            return; // stop, terminate straight away, ending thread
        }

        if (nwReply->error() == QNetworkReply::NoError)
        {
            static const QString timestampFormat("yyyy-MM-dd HH:mm:ss");
            QString xmlData = nwReply->readAll();
            QDomDocument doc;
            QDateTime updateTimestamp = QDateTime::currentDateTimeUtc();

            if (doc.setContent(xmlData))
            {
                QDomNode timestamp = doc.elementsByTagName("timestamp").at(0);
                QString ts = timestamp.toElement().text().trimmed();
                Q_ASSERT(!ts.isEmpty());

                if (!ts.isEmpty())
                {
                    // normally the timestamp is always updated from backend
                    // if this changes in the future we're prepared
                    updateTimestamp = QDateTime::fromString(ts, timestampFormat);
                    updateTimestamp.setTimeSpec(Qt::UTC);
                    if (this->getUpdateTimestamp() == updateTimestamp) return; // nothing to do
                }

                QDomNode atc = doc.elementsByTagName("atcs").at(0);
                QDomNodeList bookingNodes = atc.toElement().elementsByTagName("booking");
                int size = bookingNodes.size();
                CAtcStationList bookedStations;
                for (int i = 0; i < size; i++)
                {
                    if (this->isStopped())
                    {
                        qDebug() << "terminated" << Q_FUNC_INFO;
                        return; // stop, terminate straight away, ending thread
                    }

                    // pase nodes
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
                this->setUpdateTimestamp(updateTimestamp); // thread safe update
                emit this->dataRead(bookedStations);
            } // node
        } // content

        nwReply->close();
        nwReply->deleteLater();

    } // method

} // namespace
