/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/sequence.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/logmessage.h"
#include "vatsimbookingreader.h"

#include <QtXml/QDomElement>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackCore
{
    CVatsimBookingReader::CVatsimBookingReader(QObject *owner) :
        CThreadedReader(owner, "CVatsimBookingReader")
    {
        this->m_networkManager = new QNetworkAccessManager(this);
        this->connect(this->m_networkManager, &QNetworkAccessManager::finished, this, &CVatsimBookingReader::ps_parseBookings);
        this->connect(this->m_updateTimer, &QTimer::timeout, this, &CVatsimBookingReader::ps_read);
    }

    void CVatsimBookingReader::readInBackgroundThread()
    {
        bool s = QMetaObject::invokeMethod(this, "ps_read");
        Q_ASSERT(s);
        Q_UNUSED(s);
    }

    void CVatsimBookingReader::ps_read()
    {
        this->threadAssertCheck();
        Q_ASSERT_X(this->m_networkManager, Q_FUNC_INFO, "No network manager");
        QUrl url(m_setup.get().vatsimBookings());
        if (url.isEmpty()) { return; }

        QNetworkRequest request(url);
        this->m_networkManager->get(request);
    }

    void CVatsimBookingReader::ps_parseBookings(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

        this->threadAssertCheck();

        // Worker thread, make sure to write no members here!
        if (this->isFinished())
        {
            CLogMessage(this).debug() << Q_FUNC_INFO;
            CLogMessage(this).info("terminated booking parsing process"); // for users
            return; // stop, terminate straight away, ending thread
        }

        if (nwReply->error() == QNetworkReply::NoError)
        {
            static const QString timestampFormat("yyyy-MM-dd HH:mm:ss");
            QString xmlData = nwReply->readAll();
            nwReply->close(); // close asap
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
                    if (this->isFinished())
                    {
                        CLogMessage(this).debug() << Q_FUNC_INFO;
                        CLogMessage(this).info("Terminated booking parsing process"); // for users
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
                            bookedStation.setCallsign(CCallsign(value, CCallsign::Atc));
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
                    if (now.msecsTo(bookedStation.getBookedUntilUtc()) < (1000 * 60 * 15))     { continue; } // until n mins in past
                    if (now.msecsTo(bookedStation.getBookedFromUtc()) > (1000 * 60 * 60 * 24)) { continue; } // to far in the future, n hours
                    bookedStation.setController(user);
                    bookedStations.push_back(bookedStation);
                }
                this->setUpdateTimestamp(updateTimestamp); // thread safe update
                emit this->atcBookingsRead(bookedStations);
                emit this->dataRead(CEntityFlags::BookingEntity, CEntityFlags::ReadFinished, bookedStations.size());
            } // node
        }
        else
        {
            // network error
            CLogMessage(this).warning("Reading bookings failed %1 %2") << nwReply->errorString() << nwReply->url().toString();
            nwReply->abort();
            emit this->dataRead(CEntityFlags::BookingEntity, CEntityFlags::ReadFailed, 0);
        }
    } // method

} // namespace
