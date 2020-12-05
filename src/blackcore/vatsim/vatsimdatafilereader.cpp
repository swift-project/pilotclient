/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/vatsim/vatsimdatafilereader.h"
#include "blackcore/application.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/predicates.h"
#include "blackmisc/range.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/verify.h"

#include <QStringBuilder>
#include <QByteArray>
#include <QDateTime>
#include <QMetaObject>
#include <QNetworkReply>
#include <QReadLocker>
#include <QRegularExpression>
#include <QScopedPointer>
#include <QScopedPointerDeleteLater>
#include <QTimer>
#include <QUrl>
#include <QWriteLocker>
#include <Qt>
#include <QtGlobal>
#include <QPointer>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackCore::Data;

namespace BlackCore
{
    namespace Vatsim
    {
        CVatsimDataFileReader::CVatsimDataFileReader(QObject *owner) :
            CThreadedReader(owner, "CVatsimDataFileReader"),
            CEcosystemAware(CEcosystemAware::providerIfPossible(owner))
        {
            this->reloadSettings();
        }

        CSimulatedAircraftList CVatsimDataFileReader::getAircraft() const
        {
            QReadLocker rl(&m_lock);
            return m_aircraft;
        }

        CAtcStationList CVatsimDataFileReader::getAtcStations() const
        {
            QReadLocker rl(&m_lock);
            return m_atcStations;
        }

        CAtcStationList CVatsimDataFileReader::getAtcStationsForCallsign(const CCallsign &callsign) const
        {
            const CCallsignSet cs({callsign});
            return this->getAtcStationsForCallsigns(cs);
        }

        CAtcStationList CVatsimDataFileReader::getAtcStationsForCallsigns(const CCallsignSet &callsigns) const
        {
            return this->getAtcStations().findByCallsigns(callsigns);
        }

        CServerList CVatsimDataFileReader::getVoiceServers() const
        {
            return m_lastGoodSetup.get().getVoiceServers();
        }

        CServerList CVatsimDataFileReader::getFsdServers() const
        {
            return m_lastGoodSetup.get().getFsdServers();
        }

        CUserList CVatsimDataFileReader::getPilotsForCallsigns(const CCallsignSet &callsigns) const
        {
            return this->getAircraft().findByCallsigns(callsigns).transform(Predicates::MemberTransform(&CSimulatedAircraft::getPilot));
        }

        CUserList CVatsimDataFileReader::getPilotsForCallsign(const CCallsign &callsign) const
        {
            const CCallsignSet callsigns({callsign});
            return this->getPilotsForCallsigns(callsigns);
        }

        CAirlineIcaoCode CVatsimDataFileReader::getAirlineIcaoCode(const CCallsign &callsign) const
        {
            const CSimulatedAircraft aircraft = this->getAircraft().findFirstByCallsign(callsign);
            return aircraft.getAirlineIcaoCode();
        }

        CAircraftIcaoCode CVatsimDataFileReader::getAircraftIcaoCode(const CCallsign &callsign) const
        {
            const CSimulatedAircraft aircraft = this->getAircraft().findFirstByCallsign(callsign);
            return aircraft.getAircraftIcaoCode();
        }

        CVoiceCapabilities CVatsimDataFileReader::getVoiceCapabilityForCallsign(const CCallsign &callsign) const
        {
            if (callsign.isEmpty()) { return CVoiceCapabilities(); }
            QReadLocker rl(&m_lock);
            return m_flightPlanRemarks.value(callsign).getVoiceCapabilities();
        }

        CFlightPlanRemarks CVatsimDataFileReader::getFlightPlanRemarksForCallsign(const CCallsign &callsign) const
        {
            if (callsign.isEmpty()) { return QString(); }
            QReadLocker rl(&m_lock);
            return m_flightPlanRemarks.value(callsign);
        }

        void CVatsimDataFileReader::updateWithVatsimDataFileData(CSimulatedAircraft &aircraftToBeUdpated) const
        {
            this->getAircraft().updateWithVatsimDataFileData(aircraftToBeUdpated);
        }

        CUserList CVatsimDataFileReader::getControllersForCallsign(const CCallsign &callsign) const
        {
            const CCallsignSet cs({callsign});
            return this->getControllersForCallsigns(cs);
        }

        CUserList CVatsimDataFileReader::getControllersForCallsigns(const CCallsignSet &callsigns) const
        {
            return this->getAtcStations().findByCallsigns(callsigns).transform(Predicates::MemberTransform(&CAtcStation::getController));
        }

        CUserList CVatsimDataFileReader::getUsersForCallsign(const CCallsign &callsign) const
        {
            const CCallsignSet callsigns({callsign});
            return this->getUsersForCallsigns(callsigns);
        }

        CUserList CVatsimDataFileReader::getUsersForCallsigns(const CCallsignSet &callsigns) const
        {
            CUserList users;
            if (callsigns.isEmpty()) { return users; }
            for (const CCallsign &callsign : callsigns)
            {
                users.push_back(this->getPilotsForCallsign(callsign));
                users.push_back(this->getControllersForCallsign(callsign));
            }
            return users;
        }

        void CVatsimDataFileReader::readInBackgroundThread()
        {
            QPointer<CVatsimDataFileReader> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                myself->read();
            });
        }

        void CVatsimDataFileReader::doWorkImpl()
        {
            this->read();
        }

        void CVatsimDataFileReader::read()
        {
            this->threadAssertCheck();
            if (!this->doWorkCheck()) { return; }
            if (!this->isInternetAccessible("No network/internet access, cannot read VATSIM data file")) { return; }
            if (this->isNotVATSIMEcosystem()) { return; }

            // round robin for load balancing
            // remark: Don't use QThread to run network operations in the background
            // see http://qt-project.org/doc/qt-4.7/qnetworkaccessmanager.html
            Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing application");
            CFailoverUrlList urls(sApp->getVatsimDataFileUrls());
            const QUrl url(urls.obtainNextWorkingUrl(true));
            if (url.isEmpty()) { return; }
            this->getFromNetworkAndLog(url, { this, &CVatsimDataFileReader::parseVatsimFile});
        }

        void CVatsimDataFileReader::parseVatsimFile(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
            this->threadAssertCheck();
            if (this->isNotVATSIMEcosystem()) { return; }

            // Worker thread, make sure to write only synced here!
            if (!this->doWorkCheck())
            {
                CLogMessage(this).info(u"Terminated VATSIM file parsing process");
                return; // stop, terminate straight away, ending thread
            }

            this->logNetworkReplyReceived(nwReplyPtr);
            QStringList illegalEquipmentCodes;
            const QUrl url = nwReply->url();
            const QString urlString = url.toString();

            if (nwReply->error() == QNetworkReply::NoError)
            {
                const QString dataFileData = nwReply->readAll();
                nwReply->close(); // close asap

                if (dataFileData.isEmpty()) { return; }
                if (!this->didContentChange(dataFileData)) // Quick check by hash
                {
                    CLogMessage(this).info(u"VATSIM file '%1' has same content, skipped") << urlString;
                    return;
                }
                auto jsonDoc = QJsonDocument::fromJson(dataFileData.toUtf8());
                if (jsonDoc.isEmpty()) { return; }

                // build on local vars for thread safety
                CServerList                         fsdServers;
                CAtcStationList                     atcStations;
                CSimulatedAircraftList              aircraft;
                QMap<CCallsign, CFlightPlanRemarks> flightPlanRemarksMap;
                auto updateTimestampFromFile = QDateTime::fromString(jsonDoc["general"]["update_timestamp"].toString(), Qt::ISODateWithMs);

                const bool alreadyRead = (updateTimestampFromFile == this->getUpdateTimestamp());
                if (alreadyRead)
                {
                    CLogMessage(this).info(u"VATSIM file has same timestamp, skipped");
                    return;
                }

                for (QJsonValueRef pilot : jsonDoc["pilots"].toArray())
                {
                    if (!this->doWorkCheck())
                    {
                        CLogMessage(this).info(u"Terminated VATSIM file parsing process");
                        return;
                    }
                    aircraft.push_back(parsePilot(pilot.toObject(), illegalEquipmentCodes));
                    flightPlanRemarksMap.insert(aircraft.back().getCallsign(), parseFlightPlanRemarks(pilot.toObject()));
                }
                for (QJsonValueRef controller : jsonDoc["controllers"].toArray())
                {
                    if (!this->doWorkCheck())
                    {
                        CLogMessage(this).info(u"Terminated VATSIM file parsing process");
                        return;
                    }
                    atcStations.push_back(parseController(controller.toObject()));
                }
                for (QJsonValueRef atis : jsonDoc["atis"].toArray())
                {
                    if (!this->doWorkCheck())
                    {
                        CLogMessage(this).info(u"Terminated VATSIM file parsing process");
                        return;
                    }
                    atcStations.push_back(parseController(atis.toObject()));
                }
                for (QJsonValueRef server : jsonDoc["servers"].toArray())
                {
                    if (!this->doWorkCheck())
                    {
                        CLogMessage(this).info(u"Terminated VATSIM file parsing process");
                        return;
                    }
                    fsdServers.push_back(parseServer(server.toObject()));
                    if (!fsdServers.back().hasName()) { fsdServers.pop_back(); }
                }

                // Setup for VATSIM servers and sorting for comparison
                fsdServers.sortBy(&CServer::getName, &CServer::getDescription);

                // this part needs to be synchronized
                {
                    QWriteLocker wl(&m_lock);
                    this->setUpdateTimestamp(updateTimestampFromFile);
                    m_aircraft = aircraft;
                    m_atcStations = atcStations;
                    m_flightPlanRemarks = flightPlanRemarksMap;
                }

                // update cache itself is thread safe
                CVatsimSetup vs(m_lastGoodSetup.get());
                const bool changedSetup = vs.setServers(fsdServers, {});
                if (changedSetup)
                {
                    vs.setUtcTimestamp(updateTimestampFromFile);
                    m_lastGoodSetup.set(vs);
                }

                // warnings, if required
                if (!illegalEquipmentCodes.isEmpty())
                {
                    CVatsimDataFileReader::logInconsistentData(
                        CStatusMessage(this, CStatusMessage::SeverityInfo, u"Illegal / ignored equipment code(s) in VATSIM data file: %1") << illegalEquipmentCodes.join(", ")
                    );
                }

                // data read finished
                emit this->dataFileRead(dataFileData.size() / 1000);
                emit this->dataRead(CEntityFlags::VatsimDataFile, CEntityFlags::ReadFinished, dataFileData.size() / 1000, url);
            }
            else
            {
                // network error
                CLogMessage(this).warning(u"Reading VATSIM data file failed '%1' '%2'") << nwReply->errorString() << urlString;
                nwReply->abort();
                emit this->dataRead(CEntityFlags::VatsimDataFile, CEntityFlags::ReadFailed, 0, url);
            }
        }

        CSimulatedAircraft CVatsimDataFileReader::parsePilot(const QJsonObject &pilot, QStringList &o_illegalEquipmentCodes) const
        {
            const CCallsign callsign(pilot["callsign"].toString());
            const CUser user(pilot["cid"].toString(), pilot["name"].toString(), callsign);
            const CCoordinateGeodetic position(pilot["latitude"].toDouble(), pilot["longitude"].toDouble(), pilot["altitude"].toInt());
            const CHeading heading(pilot["heading"].toInt(), CAngleUnit::deg());
            const CSpeed groundspeed(pilot["groundspeed"].toInt(), CSpeedUnit::kts());
            const CAircraftSituation situation(callsign, position, heading, {}, {}, groundspeed);
            CSimulatedAircraft aircraft(callsign, user, situation);
            const QString icaoAndEquipment(pilot["flight_plan"]["aircraft"].toString().trimmed());
            const QString icao(CFlightPlan::aircraftIcaoCodeFromEquipmentCode(icaoAndEquipment));
            if (CAircraftIcaoCode::isValidDesignator(icao))
            {
                aircraft.setAircraftIcaoCode(icao);
            }
            else if (!icaoAndEquipment.isEmpty())
            {
                o_illegalEquipmentCodes.push_back(icaoAndEquipment);
            }
            aircraft.setTransponderCode(pilot["transponder"].toString().toInt());
            return aircraft;
        }

        CFlightPlanRemarks CVatsimDataFileReader::parseFlightPlanRemarks(const QJsonObject &pilot) const
        {
            return CFlightPlanRemarks(pilot["flight_plan"]["remarks"].toString().trimmed());
        }

        CAtcStation CVatsimDataFileReader::parseController(const QJsonObject &controller) const
        {
            const CCallsign callsign(controller["callsign"].toString());
            const CUser user(controller["cid"].toString(), controller["name"].toString(), callsign);
            const CFrequency freq(controller["frequency"].toString().toDouble(), CFrequencyUnit::kHz());
            const CLength range(controller["visual_range"].toInt(), CLengthUnit::NM());
            const QJsonArray atisLines = controller["text_atis"].toArray();
            const auto atisText = makeRange(atisLines).transform([](auto line) { return line.toString(); });
            const CInformationMessage atis(CInformationMessage::ATIS, atisText.to<QStringList>().join('\n'));
            return CAtcStation(callsign, user, freq, {}, range, true, {}, {}, atis);
        }

        CServer CVatsimDataFileReader::parseServer(const QJsonObject &server) const
        {
            return CServer(server["name"].toString(), server["location"].toString(),
                server["hostname_or_ip"].toString(), 6809, CUser("id", "real name", "email", "password"),
                CFsdSetup::vatsimStandard(), CVoiceSetup::vatsimStandard(), CEcosystem::VATSIM,
                CServer::FSDServerVatsim, server["clients_connection_allowed"].toInt());
        }

        void CVatsimDataFileReader::reloadSettings()
        {
            CReaderSettings s = m_settings.get();
            setInitialAndPeriodicTime(s.getInitialTime().toMs(), s.getPeriodicTime().toMs());
        }
    } // ns
} // ns
