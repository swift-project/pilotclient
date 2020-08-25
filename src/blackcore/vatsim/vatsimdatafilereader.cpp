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
                const QList<QStringRef> lines = splitLinesRefs(dataFileData);
                if (lines.isEmpty()) { return; }

                // build on local vars for thread safety
                CServerList                         voiceServers;
                CServerList                         fsdServers;
                CAtcStationList                     atcStations;
                CSimulatedAircraftList              aircraft;
                QMap<CCallsign, CFlightPlanRemarks> flightPlanRemarksMap;
                QDateTime                           updateTimestampFromFile;

                QStringList clientSectionAttributes;
                Section section = SectionNone;
                int invalidSections = 0;

                QString currentLine; // declared outside of the for loop, to amortize the cost of allocation
                for (const QStringRef &clRef : lines)
                {
                    if (!this->doWorkCheck())
                    {
                        CLogMessage(this).info(u"Terminated VATSIM file parsing process"); // for users
                        return; // stop, terminate straight away, ending thread
                    }

                    // parse lines
                    currentLine = clRef.toString().trimmed();
                    if (currentLine.isEmpty()) continue;
                    if (currentLine.startsWith(";"))
                    {
                        if (clientSectionAttributes.isEmpty() && currentLine.contains("!CLIENTS SECTION", Qt::CaseInsensitive))
                        {
                            // ; !CLIENTS section
                            const int i = currentLine.lastIndexOf(' ');
                            const QVector<QStringRef> attributes = currentLine.midRef(i).trimmed().split(':', Qt::SkipEmptyParts);
                            for (const QStringRef &attr : attributes) { clientSectionAttributes.push_back(attr.toString().trimmed().toLower()); }
                            section = SectionNone; // reset

                            // consistency check to avoid tons of parsing errors afterwards
                            // normally we have 40 attributes
                            if (attributes.size() < 10)
                            {
                                CLogMessage(this).warning(u"Too few (%1) attributes in VATSIM file, CANCEL parsing. Line: '%2'") << attributes.size() << currentLine;
                                return;
                            }

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
                            const bool logInconsistencies = invalidSections < 5; // flood protection
                            const QMap<QString, QString> clientPartsMap = clientPartsToMap(currentLine, clientSectionAttributes, logInconsistencies);
                            const CCallsign callsign = CCallsign(clientPartsMap["callsign"]);
                            if (callsign.isEmpty())
                            {
                                invalidSections++;
                                break;
                            }
                            const CUser user(clientPartsMap["cid"], clientPartsMap["realname"], callsign);
                            const QString clientType = clientPartsMap["clienttype"].toLower();
                            if (clientType.isEmpty()) { break; } // sometimes type is empty

                            bool ok;
                            bool validPos = true;
                            QStringList posMsg;
                            const double lat = clientPartsMap["latitude"].toDouble(&ok);
                            if (!ok) { validPos = false; posMsg << QStringLiteral("latitude: '%1'").arg(clientPartsMap["latitude"]); }

                            const double lng = clientPartsMap["longitude"].toDouble(&ok);
                            if (!ok) { validPos = false; posMsg << QStringLiteral("longitude: '%1'").arg(clientPartsMap["longitude"]); }

                            const double alt = clientPartsMap["altitude"].toDouble(&ok);
                            if (!ok) { validPos = false; posMsg << QStringLiteral("altitude: '%1'").arg(clientPartsMap["altitude"]); }
                            const CCoordinateGeodetic position = validPos ? CCoordinateGeodetic(lat, lng, alt) : CCoordinateGeodetic::null();

                            Q_ASSERT_X((validPos && posMsg.isEmpty()) || (!validPos && !posMsg.isEmpty()), Q_FUNC_INFO, "Inconsistent data");
                            if (!posMsg.isEmpty())
                            {
                                // Only info not to flood lof with warning
                                CLogMessage(this).validationInfo(u"Callsign '%1' %2 (VATSIM data file)") << callsign << posMsg.join(", ");
                            }

                            const CFrequency frequency = CFrequency(clientPartsMap["frequency"].toDouble(), CFrequencyUnit::MHz());
                            const QString flightPlanRemarks = clientPartsMap["planned_remarks"].trimmed();

                            // Voice capabilities
                            if (!flightPlanRemarks.isEmpty())
                            {
                                // CFlightPlanRemarks contains voice capabilities and other parsed values
                                flightPlanRemarksMap[callsign] = CFlightPlanRemarks(flightPlanRemarks);
                            }

                            // set as per ATC/pilot
                            if (clientType.startsWith('p'))
                            {
                                // Pilot section
                                const double groundSpeedKts = clientPartsMap["groundspeed"].toDouble();
                                CAircraftSituation situation(position);
                                situation.setGroundSpeed(CSpeed(groundSpeedKts, CSpeedUnit::kts()));
                                CSimulatedAircraft currentAircraft(user.getCallsign().getStringAsSet(), user, situation);

                                const QString equipmentCodeAndAircraft = clientPartsMap["planned_aircraft"].trimmed();
                                if (!equipmentCodeAndAircraft.isEmpty())
                                {
                                    const QString aircraftIcaoCode = CFlightPlan::aircraftIcaoCodeFromEquipmentCode(equipmentCodeAndAircraft);
                                    if (CAircraftIcaoCode::isValidDesignator(aircraftIcaoCode))
                                    {
                                        currentAircraft.setAircraftIcaoDesignator(aircraftIcaoCode);
                                    }
                                    else
                                    {
                                        illegalEquipmentCodes.append(equipmentCodeAndAircraft);
                                    }
                                }
                                aircraft.push_back(currentAircraft);
                            }
                            else if (clientType.startsWith('a'))
                            {
                                // ATC section
                                CLength range;
                                // should be alread have alt/height position.setGeodeticHeight(altitude);
                                // the altitude is elevation for a station
                                CAtcStation station(user.getCallsign().getStringAsSet(), user, frequency, position, range);
                                station.setOnline(true);
                                atcStations.push_back(station);
                            }
                            else
                            {
                                BLACK_VERIFY_X(false, Q_FUNC_INFO, "Wrong client type");
                                break;
                            }
                        }
                        break;
                    case SectionGeneral:
                        {
                            if (currentLine.contains("UPDATE"))
                            {
                                const QStringList updateParts = currentLine.replace(" ", "").split('=');
                                if (updateParts.length() < 2) { break; }
                                const QString dts = updateParts.at(1).trimmed();
                                updateTimestampFromFile = fromStringUtc(dts, "yyyyMMddHHmmss");
                                const bool alreadyRead = (updateTimestampFromFile == this->getUpdateTimestamp());
                                if (alreadyRead)
                                {
                                    CLogMessage(this).info(u"VATSIM file has same timestamp, skipped");
                                    return;
                                }
                            }
                        }
                        break;
                    case SectionFsdServers:
                        {
                            // ident:hostname_or_IP:location:name:clients_connection_allowed:
                            const QStringList fsdServerParts = currentLine.split(':');
                            if (fsdServerParts.size() < 5) { break; }
                            if (!fsdServerParts.at(4).trimmed().contains('1')) { break; } // allowed?
                            const QString description(fsdServerParts.at(2)); // part(3) could be added
                            const CServer fsdServer(fsdServerParts.at(0), description, fsdServerParts.at(1), 6809,
                                                    CUser("id", "real name", "email", "password"),
                                                    CFsdSetup::vatsimStandard(), CVoiceSetup::vatsimStandard(),
                                                    CEcosystem(CEcosystem::VATSIM), CServer::FSDServerVatsim);
                            fsdServers.push_back(fsdServer);
                        }
                        break;
                    case SectionVoiceServers:
                        {
                            // hostname_or_IP:location:name:clients_connection_allowed:type_of_voice_server:
                            const QStringList voiceServerParts = currentLine.split(':');
                            if (voiceServerParts.size() < 4) { break; }
                            if (!voiceServerParts.at(3).trimmed().contains('1')) { break; } // allowed?
                            const CServer voiceServer(voiceServerParts.at(1), voiceServerParts.at(2), voiceServerParts.at(0), -1,
                                                      CUser(),
                                                      CFsdSetup(), CVoiceSetup::vatsimStandard(),
                                                      CEcosystem(CEcosystem::VATSIM), CServer::VoiceServerVatsim);
                            voiceServers.push_back(voiceServer);
                        }
                        break;
                    case SectionNone:
                    default:
                        break;

                    } // switch section
                } // for each line

                // Setup for VATSIM servers and sorting for comparison
                fsdServers.sortBy(&CServer::getName, &CServer::getDescription);
                voiceServers.sortBy(&CServer::getName, &CServer::getDescription);

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
                const bool changedSetup = vs.setServers(fsdServers, voiceServers);
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
                emit this->dataFileRead(lines.count());
                emit this->dataRead(CEntityFlags::VatsimDataFile, CEntityFlags::ReadFinished, lines.count(), url);
            }
            else
            {
                // network error
                CLogMessage(this).warning(u"Reading VATSIM data file failed '%1' '%2'") << nwReply->errorString() << urlString;
                nwReply->abort();
                emit this->dataRead(CEntityFlags::VatsimDataFile, CEntityFlags::ReadFailed, 0, url);
            }
        }

        void CVatsimDataFileReader::reloadSettings()
        {
            CReaderSettings s = m_settings.get();
            setInitialAndPeriodicTime(s.getInitialTime().toMs(), s.getPeriodicTime().toMs());
        }

        const QMap<QString, QString> CVatsimDataFileReader::clientPartsToMap(const QString &currentLine, const QStringList &clientSectionAttributes, bool logInconsistency)
        {
            QMap<QString, QString> parts;
            if (currentLine.isEmpty()) { return parts; }
            QStringList clientParts = currentLine.split(':');

            // remove last empty item if required
            if (currentLine.endsWith(':')) { clientParts.removeLast(); }
            const int noParts = clientParts.size();
            const int noAttributes = clientSectionAttributes.size();
            const bool valid = (noParts == noAttributes);

            // valid data?
            if (!valid)
            {
                if (logInconsistency)
                {
                    logInconsistentData(
                        CStatusMessage(static_cast<CVatsimDataFileReader *>(nullptr), CStatusMessage::SeverityInfo, u"VATSIM data file client parts: %1 attributes: %2 line: '%3'") << clientParts.size() << clientSectionAttributes.size() << currentLine);
                }
                return parts;
            }

            for (int i = 0; i < clientSectionAttributes.size(); i++)
            {
                // section attributes are the column names
                const QString attribute(clientSectionAttributes.at(i));
                parts.insert(attribute, clientParts.at(i));
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
    } // ns
} // ns
