/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
#include "blackmisc/verify.h"

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
#include <QPointer>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackCore::Vatsim
{
    CVatsimBookingReader::CVatsimBookingReader(QObject *owner) :
        CThreadedReader(owner, "CVatsimBookingReader"),
        CEcosystemAware(CEcosystemAware::providerIfPossible(owner))
    {
        this->settingsChanged();
    }

    void CVatsimBookingReader::readInBackgroundThread()
    {
        QPointer<CVatsimBookingReader> myself(this);
        QTimer::singleShot(0, this, [ = ]
        {
            if (!myself) { return; }
            myself->read();
        });
    }

    void CVatsimBookingReader::doWorkImpl()
    {
        this->read();
    }

    void CVatsimBookingReader::read()
    {
        this->threadAssertCheck();
        if (!this->doWorkCheck()) { return; }
        if (!this->isInternetAccessible("No network/internet access, cannot read VATSIM bookings")) { return; }
        if (this->isNotVATSIMEcosystem()) { return; }

        Q_ASSERT_X(sApp, Q_FUNC_INFO, "No application");
        const QUrl url(sApp->getGlobalSetup().getVatsimBookingsUrl());
        if (url.isEmpty()) { return; }
        this->getFromNetworkAndLog(url, { this, &CVatsimBookingReader::parseBookings});
    }

    void CVatsimBookingReader::parseBookings(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        this->threadAssertCheck();
        if (this->isNotVATSIMEcosystem()) { return; }

        // Worker thread, make sure to write no members here od do it threadsafe
        if (!this->doWorkCheck())
        {
            CLogMessage(this).info(u"Terminated booking parsing process");
            return; // stop, terminate straight away, ending thread
        }

        const CReaderSettings settings = m_settings.get();
        this->logNetworkReplyReceived(nwReplyPtr);
        const QUrl url = nwReply->url();

        if (nwReply->error() == QNetworkReply::NoError)
        {
            static const QString timestampFormat("yyyy-MM-dd HH:mm:ss");
            const QString xmlData = nwReply->readAll();
            nwReply->close(); // close asap

            if (xmlData.isEmpty() || !xmlData.contains("<bookings>", Qt::CaseInsensitive))
            {
                CLogMessage(this).warning(u"Reading bookings wrong XML format for '%1'") << nwReply->url().toString();
                m_failures++;
                emit this->dataRead(CEntityFlags::BookingEntity, CEntityFlags::ReadFailed, 0, url);
                return;
            }

            QDomDocument doc;
            QDateTime updateTimestamp = QDateTime::currentDateTimeUtc(); // default
            if (doc.setContent(xmlData))
            {
                const QDomNode timestamp = doc.elementsByTagName("timestamp").at(0);
                const QString ts = timestamp.toElement().text().trimmed();
                BLACK_AUDIT_X(!ts.isEmpty(), Q_FUNC_INFO, "Wrong timestamp format");

                if (ts.isEmpty())
                {
                    CLogMessage(this).warning(u"Reading bookings wrong XML timestamp format for '%1'") << nwReply->url().toString();
                    m_failures++;
                    emit this->dataRead(CEntityFlags::BookingEntity, CEntityFlags::ReadFailed, 0, url);
                    return;
                }
                else
                {
                    // normally the timestamp is always updated from backend
                    // if this changes in the future we're prepared
                    updateTimestamp = fromStringUtc(ts, timestampFormat);
                    if (this->getUpdateTimestamp() == updateTimestamp) return; // nothing to do

                    // save parsing and all follow up actions if nothing changed
                    bool changed = this->didContentChange(xmlData, xmlData.indexOf("</timestamp>"));
                    if (!changed)
                    {
                        CLogMessage(this).info(u"Read bookings unchanged, skipped");
                        emit this->atcBookingsReadUnchanged();
                        return; // stop, terminate straight away, ending thread
                    }
                }

                const QDomNode atc = doc.elementsByTagName("atcs").at(0);
                const QDomNodeList bookingNodes = atc.toElement().elementsByTagName("booking");
                int size = bookingNodes.size();
                CAtcStationList bookedStations;
                for (int i = 0; i < size; i++)
                {
                    if (!this->doWorkCheck())
                    {
                        CLogMessage(this).info(u"Terminated booking parsing process"); // for users
                        return; // stop, terminate straight away, ending thread
                    }

                    // pase nodes
                    const QDomNode bookingNode = bookingNodes.at(i);
                    const QDomNodeList bookingNodeValues = bookingNode.childNodes();
                    CAtcStation bookedStation;
                    CUser user;
                    for (int v = 0; v < bookingNodeValues.size(); v++)
                    {
                        const QDomNode bookingNodeValue = bookingNodeValues.at(v);
                        const QString name = bookingNodeValue.nodeName().toLower();
                        const QString value = bookingNodeValue.toElement().text();
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
                            const QDateTime t = fromStringUtc(value, timestampFormat);
                            bookedStation.setBookedUntilUtc(t);
                        }
                        else if (name == "time_start")
                        {
                            const QDateTime t = fromStringUtc(value, timestampFormat);
                            bookedStation.setBookedFromUtc(t);
                        }
                    }
                    // time checks
                    const QDateTime now = QDateTime::currentDateTimeUtc();
                    if (now.msecsTo(bookedStation.getBookedUntilUtc()) < (1000 * 60 * 15))      { continue; } // until n mins in past
                    if (now.msecsTo(bookedStation.getBookedFromUtc())  > (1000 * 60 * 60 * 24)) { continue; } // to far in the future, n hours
                    bookedStation.setController(user);
                    bookedStations.push_back(bookedStation);
                }
                m_failures = 0;
                this->setUpdateTimestamp(updateTimestamp); // thread safe update
                this->setInitialAndPeriodicTime(settings.getInitialTime().toMs(), 3 * settings.getPeriodicTime().toMs()); // slow down, we have some bookings now

                emit this->atcBookingsRead(bookedStations);
                emit this->dataRead(CEntityFlags::BookingEntity, CEntityFlags::ReadFinished, bookedStations.size(), url);
            } // node
        }
        else
        {
            // network error
            CLogMessage(this).warning(u"Reading bookings failed '%1' '%2'") << nwReply->errorString() << url.toString();
            nwReply->abort();
            m_failures++;
            if (m_failures > 3)
            {
                // slow updates from now on
                this->setInitialAndPeriodicTime(settings.getInitialTime().toMs(), 10 * settings.getPeriodicTime().toMs()); // massively slow down
                CLogMessage(this).warning(u"Too many booking reader failures %1, slower updates") << m_failures;
            }
            emit this->dataRead(CEntityFlags::BookingEntity, CEntityFlags::ReadFailed, 0, url);
        }
    } // method

    void CVatsimBookingReader::settingsChanged()
    {
        const CReaderSettings s = m_settings.get();
        this->setInitialAndPeriodicTime(s.getInitialTime().toMs(), s.getPeriodicTime().toMs());
    }
} // ns
