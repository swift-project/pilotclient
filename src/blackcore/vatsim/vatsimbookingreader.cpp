/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/vatsim/vatsimbookingreader.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/user.h"
#include "blackmisc/statusmessage.h"

#include <QByteArray>
#include <QDateTime>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QMetaObject>
#include <QNetworkReply>
#include <QScopedPointer>
#include <QScopedPointerDeleteLater>
#include <QString>
#include <QTimer>
#include <QUrl>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackCore
{
    namespace Vatsim
    {
        CVatsimBookingReader::CVatsimBookingReader(QObject *owner) :
            CThreadedReader(owner, "CVatsimBookingReader")
        {
            this->connect(this->m_updateTimer, &QTimer::timeout, this, &CVatsimBookingReader::ps_read);
        }

        void CVatsimBookingReader::readInBackgroundThread()
        {
            bool s = QMetaObject::invokeMethod(this, "ps_read");
            Q_ASSERT(s);
            Q_UNUSED(s);
        }

        void CVatsimBookingReader::cleanup()
        {
            // void
        }

        Settings::CSettingsReader CVatsimBookingReader::getSettings() const
        {
            return this->m_settings.get();
        }

        void CVatsimBookingReader::ps_read()
        {
            this->threadAssertCheck();
            Q_ASSERT_X(sApp, Q_FUNC_INFO, "No application");
            const QUrl url(sApp->getGlobalSetup().getVatsimBookingsUrl());
            if (url.isEmpty()) { return; }

            sApp->getFromNetwork(url, { this, &CVatsimBookingReader::ps_parseBookings});
        }

        void CVatsimBookingReader::ps_parseBookings(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

            this->threadAssertCheck();

            // Worker thread, make sure to write no members here od do it threadsafe
            if (this->isAbandoned())
            {
                CLogMessage(this).debug() << Q_FUNC_INFO;
                CLogMessage(this).info("Terminated booking parsing process");
                return; // stop, terminate straight away, ending thread
            }

            if (nwReply->error() == QNetworkReply::NoError)
            {
                static const QString timestampFormat("yyyy-MM-dd HH:mm:ss");
                const QString xmlData = nwReply->readAll();
                nwReply->close(); // close asap

                QDomDocument doc;
                QDateTime updateTimestamp = QDateTime::currentDateTimeUtc(); // default
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

                        // save parsing and all follow up actions if nothing changed
                        bool changed = this->didContentChange(xmlData, xmlData.indexOf("</timestamp>"));
                        if (!changed)
                        {
                            CLogMessage(this).info("Bookings unchanged, skipped");
                            return; // stop, terminate straight away, ending thread
                        }
                    }

                    QDomNode atc = doc.elementsByTagName("atcs").at(0);
                    QDomNodeList bookingNodes = atc.toElement().elementsByTagName("booking");
                    int size = bookingNodes.size();
                    CAtcStationList bookedStations;
                    for (int i = 0; i < size; i++)
                    {
                        if (this->isAbandoned())
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
    } // ns
} // ns
