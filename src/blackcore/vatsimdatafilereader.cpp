/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/sequence.h"
#include "blackmisc/avatcstation.h"
#include "blackmisc/nwuser.h"
#include "blackmisc/nwserver.h"
#include "blackmisc/logmessage.h"
#include "vatsimdatafilereader.h"

#include <QRegularExpression>
#include <QtConcurrent/QtConcurrent>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCore
{
    CVatsimDataFileReader::CVatsimDataFileReader(const QStringList &urls, QObject *parent) :
        QObject(parent), CThreadedReader(),
        m_serviceUrls(urls), m_currentUrlIndex(0), m_networkManager(nullptr)
    {
        this->m_networkManager = new QNetworkAccessManager(this);
        this->connect(this->m_networkManager, &QNetworkAccessManager::finished, this, &CVatsimDataFileReader::ps_loadFinished);
        this->connect(this->m_updateTimer, &QTimer::timeout, this, &CVatsimDataFileReader::read);
    }

    void CVatsimDataFileReader::read()
    {
        if (this->m_serviceUrls.isEmpty()) return;

        // round robin for load distribution
        this->m_currentUrlIndex++;
        if (this->m_serviceUrls.size() >= this->m_currentUrlIndex) this->m_currentUrlIndex = 0;

        // remark: Don't use QThread to run network operations in the background
        // see http://qt-project.org/doc/qt-4.7/qnetworkaccessmanager.html
        QUrl url(this->m_serviceUrls.at(this->m_currentUrlIndex));
        if (url.isEmpty()) return;
        Q_ASSERT(this->m_networkManager);
        QNetworkRequest request(url);
        QNetworkReply *r = this->m_networkManager->get(request);
        this->setPendingNetworkReply(r);
    }

    CAircraftList CVatsimDataFileReader::getAircrafts() const
    {
        QReadLocker rl(&this->m_lock);
        return this->m_aircrafts;
    }

    CAtcStationList CVatsimDataFileReader::getAtcStations() const
    {
        QReadLocker rl(&this->m_lock);
        return this->m_atcStations;
    }

    CServerList CVatsimDataFileReader::getVoiceServers() const
    {
        QReadLocker rl(&this->m_lock);
        return this->m_voiceServers;
    }

    CServerList CVatsimDataFileReader::getFsdServers() const
    {
        QReadLocker rl(&this->m_lock);
        return this->m_fsdServers;
    }

    CUserList CVatsimDataFileReader::getPilotsForCallsigns(const CCallsignList &callsigns)
    {
        return this->getAircrafts().findByCallsigns(callsigns).transform(Predicates::MemberTransform(&CAircraft::getPilot));
    }

    CUserList CVatsimDataFileReader::getPilotsForCallsign(const CCallsign &callsign)
    {
        CCallsignList callsigns;
        callsigns.push_back(callsign);
        return this->getPilotsForCallsigns(callsigns);
    }

    CAircraftIcao CVatsimDataFileReader::getIcaoInfo(const CCallsign &callsign)
    {
        CAircraft aircraft = this->getAircrafts().findFirstByCallsign(callsign);
        return aircraft.getIcaoInfo();
    }

    CVoiceCapabilities CVatsimDataFileReader::getVoiceCapabilityForCallsign(const CCallsign &callsign)
    {
        QReadLocker rl(&this->m_lock);
        if (this->m_voiceCapabilities.contains(callsign))
        {
            return m_voiceCapabilities[callsign];
        }
        else
        {
            return CVoiceCapabilities::fromVoiceCapabilities(CVoiceCapabilities::Unknown);
        }
    }

    void CVatsimDataFileReader::updateWithVatsimDataFileData(CAircraft &aircraftToBeUdpated) const
    {
        this->getAircrafts().updateWithVatsimDataFileData(aircraftToBeUdpated);
    }

    CUserList CVatsimDataFileReader::getControllersForCallsign(const CCallsign &callsign)
    {
        CCallsignList callsigns;
        callsigns.push_back(callsign);
        return this->getControllersForCallsigns(callsigns);
    }

    CUserList CVatsimDataFileReader::getUsersForCallsign(const CCallsign &callsign)
    {
        CCallsignList callsigns;
        callsigns.push_back(callsign);
        return this->getUsersForCallsigns(callsigns);
    }

    CUserList CVatsimDataFileReader::getControllersForCallsigns(const CCallsignList &callsigns)
    {
        return this->getAtcStations().findByCallsigns(callsigns).transform(Predicates::MemberTransform(&CAtcStation::getController));
    }

    CUserList CVatsimDataFileReader::getUsersForCallsigns(const CCallsignList &callsigns)
    {
        CUserList users;
        if (callsigns.isEmpty()) return users;
        foreach(CCallsign callsign, callsigns)
        {
            users.push_back(this->getPilotsForCallsign(callsign));
            users.push_back(this->getControllersForCallsign(callsign));
        }
        return users;
    }

    /*
     * Data file read from XML
     */
    void CVatsimDataFileReader::ps_loadFinished(QNetworkReply *nwReply)
    {
        this->setPendingNetworkReply(nullptr);
        if (!this->isStopped())
        {
            QFuture<void> f = QtConcurrent::run(this, &CVatsimDataFileReader::parseVatsimFileInBackground, nwReply);
            this->setPendingFuture(f);
        }
    }

    /*
     * Data file read from XML
     * Example: http://info.vroute.net/vatsim-data.txt
     */
    void CVatsimDataFileReader::parseVatsimFileInBackground(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

        // Worker thread, make sure to write only synced here!
        if (this->isStopped())
        {
            CLogMessage(this).debug() << Q_FUNC_INFO;
            CLogMessage(this).info("terminated VATSIM file parsing process"); // for users
            return; // stop, terminate straight away, ending thread
        }

        QStringList illegalIcaoCodes;
        if (nwReply->error() == QNetworkReply::NoError)
        {
            const QString dataFileData = nwReply->readAll();
            nwReply->close(); // close asap

            if (dataFileData.isEmpty()) return;
            QStringList lines = dataFileData.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
            if (lines.isEmpty()) return;

            // build on local vars for thread safety
            CServerList     voiceServers;
            CServerList     fsdServers;
            CAtcStationList atcStations;
            CAircraftList   aircrafts;
            QMap<CCallsign, CVoiceCapabilities> voiceCapabilities;
            QDateTime updateTimestampFromFile;

            QStringList clientSectionAttributes;
            Section section = SectionNone;
            foreach(QString currentLine, lines)
            {
                if (this->isStopped())
                {
                    CLogMessage(this).debug() << Q_FUNC_INFO;
                    CLogMessage(this).info("terminated booking parsing process"); // for users
                    return; // stop, terminate straight away, ending thread
                }

                // parse lines
                currentLine = currentLine.trimmed();
                if (currentLine.isEmpty()) continue;
                if (currentLine.startsWith(";"))
                {
                    if (clientSectionAttributes.isEmpty() && currentLine.contains("!CLIENTS SECTION", Qt::CaseInsensitive))
                    {
                        // ; !CLIENTS section
                        int i = currentLine.lastIndexOf(' ');
                        QString attributes = currentLine.mid(i).trimmed();
                        clientSectionAttributes = attributes.split(':', QString::SkipEmptyParts);
                        section = SectionNone; // reset
                    }
                    continue;
                }
                else if (currentLine.startsWith("!"))
                {
                    section = currentLineToSection(currentLine);
                    continue;
                }

                switch (section)
                {
                case SectionClients:
                    {
                        QMap<QString, QString> clientPartsMap = clientPartsToMap(currentLine, clientSectionAttributes);
                        CCallsign callsign = CCallsign(clientPartsMap["callsign"]);
                        if (callsign.isEmpty()) continue;
                        BlackMisc::Network::CUser user(clientPartsMap["cid"], clientPartsMap["realname"], callsign);
                        const QString clientType = clientPartsMap["clienttype"].toLower();
                        if (clientType.isEmpty()) break; // sometimes type is empty
                        double lat = clientPartsMap["latitude"].toDouble();
                        double lng = clientPartsMap["longitude"].toDouble();
                        double alt = clientPartsMap["altitude"].toDouble();
                        CFrequency frequency = CFrequency(clientPartsMap["frequency"].toDouble(), CFrequencyUnit::MHz());
                        CCoordinateGeodetic position(lat, lng, -1);
                        CAltitude altitude(alt, CAltitude::MeanSeaLevel, CLengthUnit::ft());
                        QString flightPlanRemarks = clientPartsMap["planned_remarks"];

                        // Voice capabilities
                        if (!flightPlanRemarks.isEmpty())
                        {
                            CVoiceCapabilities vc(flightPlanRemarks);
                            if (!vc.isUnknown())
                            {
                                voiceCapabilities.insert(callsign, vc);
                            }
                        }

                        // set as per ATC/pilot
                        if (clientType.startsWith('p'))
                        {
                            // Pilot section
                            double groundspeed = clientPartsMap["groundspeed"].toDouble();
                            CAircraftSituation situation(position, altitude);
                            situation.setGroundspeed(CSpeed(groundspeed, CSpeedUnit::kts()));
                            CAircraft aircraft(user.getCallsign().getStringAsSet(), user, situation);

                            QString icaoCode = clientPartsMap["planned_aircraft"];
                            if (!icaoCode.isEmpty())
                            {
                                // http://uk.flightaware.com/about/faq_aircraft_flight_plan_suffix.rvt
                                // we expect something like H/B772/F B773 B773/F
                                static const QRegularExpression reg("/.");
                                icaoCode = icaoCode.replace(reg, "").trimmed().toUpper();
                                if (CAircraftIcao::isValidDesignator(icaoCode))
                                {
                                    aircraft.setIcaoInfo(CAircraftIcao(icaoCode));
                                }
                                else
                                {
                                    illegalIcaoCodes.append(icaoCode);
                                }
                            }

                            aircrafts.push_back(aircraft);
                        }
                        else if (clientType.startsWith('a'))
                        {
                            // ATC section
                            CLength range;
                            position.setGeodeticHeight(altitude); // the altitude is elevation for a station
                            CAtcStation station(user.getCallsign().getStringAsSet(), user, frequency, position, range);
                            station.setOnline(true);
                            atcStations.push_back(station);
                        }
                        else
                        {
                            Q_ASSERT_X(false, "CVatsimDataFileReader::loadFinished", "Wrong client type");
                        }
                    }
                    break;
                case SectionGeneral:
                    {
                        if (currentLine.contains("UPDATE"))
                        {
                            QStringList updateParts = currentLine.replace(" ", "").split('=');
                            if (updateParts.length() < 2) continue;
                            QString dts = updateParts.at(1).trimmed();
                            updateTimestampFromFile = QDateTime::fromString(dts, "yyyyMMddHHmmss");
                            updateTimestampFromFile.setOffsetFromUtc(0);
                            bool alreadyRead = (updateTimestampFromFile == this->getUpdateTimestamp());
                            if (alreadyRead) { return; }// still same data, terminate
                        }
                    }
                    break;
                case SectionFsdServers:
                    {
                        // ident:hostname_or_IP:location:name:clients_connection_allowed:
                        QStringList fsdServerParts = currentLine.split(':');
                        if (fsdServerParts.size() < 4) continue;
                        if (!fsdServerParts.at(3).trimmed().contains('1')) continue; // allowed?
                        BlackMisc::Network::CServer fsdServer(fsdServerParts.at(0), fsdServerParts.at(2), fsdServerParts.at(1), 6809, CUser("id", "real name", "email", "password"));
                        fsdServers.push_back(fsdServer);
                    }
                    break;
                case SectionVoiceServers:
                    {
                        // hostname_or_IP:location:name:clients_connection_allowed:type_of_voice_server:
                        QStringList voiceServerParts = currentLine.split(':');
                        if (voiceServerParts.size() < 3) continue;
                        if (!voiceServerParts.at(3).trimmed().contains('1')) continue; // allowed?
                        BlackMisc::Network::CServer voiceServer(voiceServerParts.at(1), voiceServerParts.at(2), voiceServerParts.at(0), -1, CUser());
                        voiceServers.push_back(voiceServer);
                    }
                    break;
                case SectionNone:
                default:
                    break;

                } // switch section

                // this part needs to be synchronized
                {
                    QWriteLocker wl(&this->m_lock);
                    this->setUpdateTimestamp(updateTimestampFromFile);
                    this->m_aircrafts = aircrafts;
                    this->m_atcStations = atcStations;
                    this->m_voiceServers = voiceServers;
                    this->m_fsdServers = fsdServers;
                    this->m_voiceCapabilities = voiceCapabilities;
                }
            } // for each line

            // warnings, if required
            if (!illegalIcaoCodes.isEmpty())
            {
                CLogMessage(this).info("Illegal / ignored ICAO code(s) in VATSIM data file: %1") << illegalIcaoCodes.join(", ");
            }

            // data read finished
            emit this->dataRead();
        }
        else
        {
            // network error
            nwReply->abort();
        }

    }

    const QMap<QString, QString> CVatsimDataFileReader::clientPartsToMap(const QString &currentLine, const QStringList &clientSectionAttributes)
    {
        QStringList clientParts = currentLine.split(':');
        QMap<QString, QString> parts;
        for (int i = 0; i < clientSectionAttributes.size(); i++)
        {
            Q_ASSERT(i < clientSectionAttributes.size());
            Q_ASSERT(i < clientParts.size());
            parts.insert(clientSectionAttributes.at(i).toLower(), clientParts.at(i));
        }
        return parts;
    }

    CVatsimDataFileReader::Section CVatsimDataFileReader::currentLineToSection(const QString &currentLine)
    {
        if (currentLine.contains("!GENERAL", Qt::CaseInsensitive)) { return SectionGeneral; }
        if (currentLine.contains("!VOICE SERVERS", Qt::CaseInsensitive)) { return SectionVoiceServers; }
        if (currentLine.contains("!SERVERS", Qt::CaseInsensitive)) { return SectionFsdServers; }
        if (currentLine.contains("!CLIENTS", Qt::CaseInsensitive)) { return SectionClients; }
        return SectionNone;
    }
} // namespace
