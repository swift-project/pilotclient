#include "blackmisc/sequence.h"
#include "blackmisc/avatcstation.h"
#include "blackmisc/nwuser.h"
#include "blackmisc/nwserver.h"
#include "vatsimdatafilereader.h"

#include <QRegularExpression>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;


namespace BlackCore
{

    CVatsimDataFileReader::CVatsimDataFileReader(const QStringList &urls, QObject *parent) : QObject(parent), m_serviceUrls(urls), m_currentUrlIndex(0), m_networkManager(nullptr), m_updateTimer(nullptr)
    {
        this->m_networkManager = new QNetworkAccessManager(this);
        this->m_updateTimer = new QTimer(this);
        this->connect(this->m_networkManager, &QNetworkAccessManager::finished, this, &CVatsimDataFileReader::loadFinished);
        this->connect(this->m_updateTimer, &QTimer::timeout, this, &CVatsimDataFileReader::read);
    }

    void CVatsimDataFileReader::read()
    {
        if (this->m_serviceUrls.isEmpty()) return;
        this->m_currentUrlIndex++;
        if (this->m_serviceUrls.size() >= this->m_currentUrlIndex) this->m_currentUrlIndex = 0;

        QUrl url(this->m_serviceUrls.at(this->m_currentUrlIndex));
        if (url.isEmpty()) return;
        Q_ASSERT(this->m_networkManager);
        QNetworkRequest request(url);
        this->m_networkManager->get(request);
    }

    void CVatsimDataFileReader::setInterval(int updatePeriodMs)
    {
        Q_ASSERT(this->m_updateTimer);
        if (updatePeriodMs < 1)
            this->m_updateTimer->stop();
        else
            this->m_updateTimer->start(updatePeriodMs);
    }

    CUserList CVatsimDataFileReader::getPilotsForCallsigns(const CCallsignList &callsigns)
    {
        CUserList users;
        if (callsigns.isEmpty()) return users;
        foreach(CCallsign callsign, callsigns)
        {
            users.push_back(this->m_aircrafts.findByCallsign(callsign).getPilots());
        }
        return users;
    }

    CUserList CVatsimDataFileReader::getPilotsForCallsign(const CCallsign &callsign)
    {
        CCallsignList callsigns;
        callsigns.push_back(callsign);
        return this->getPilotsForCallsigns(callsigns);
    }

    CAircraftIcao CVatsimDataFileReader::getIcaoInfo(const CCallsign &callsign)
    {
        CAircraft aircraft = this->m_aircrafts.findFirstByCallsign(callsign);
        return aircraft.getIcaoInfo();
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
        CUserList users;
        if (callsigns.isEmpty()) return users;
        foreach(CCallsign callsign, callsigns)
        {
            users.push_back(this->m_atcStations.findByCallsign(callsign).getControllers());
        }
        return users;
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
    void CVatsimDataFileReader::loadFinished(QNetworkReply *nwReply)
    {
        // Example: http://info.vroute.net/vatsim-data.txt
        if (nwReply->error() == QNetworkReply::NoError)
        {
            const QString dataFileData = nwReply->readAll();
            if (dataFileData.isEmpty()) return;
            QStringList lines = dataFileData.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
            if (lines.isEmpty()) return;

            QStringList clientSectionAttributes;
            Section section = SectionNone;
            foreach(QString currentLine, lines)
            {
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
                    if (currentLine.contains("GENERAL", Qt::CaseInsensitive))
                    {
                        section = SectionGeneral;
                    }
                    else if (currentLine.contains("VOICE SERVERS", Qt::CaseInsensitive))
                    {
                        section = SectionVoiceServer;
                        this->m_voiceServers.clear();
                    }
                    else if (currentLine.contains("CLIENTS", Qt::CaseInsensitive))
                    {
                        section = SectionClients;
                        this->m_aircrafts.clear();
                        this->m_atcStations.clear();
                    }
                    else
                    {
                        section = SectionNone;
                    }
                    continue;
                }
                switch (section)
                {
                case SectionClients:
                    {
                        QMap<QString, QString> clientPartsMap = clientPartsToMap(currentLine, clientSectionAttributes);
                        BlackMisc::Network::CUser user(clientPartsMap["cid"], clientPartsMap["realname"], CCallsign(clientPartsMap["callsign"]));
                        if (!user.hasValidCallsign()) continue;
                        const QString clientType = clientPartsMap["clienttype"].toLower();
                        if (clientType.isEmpty()) break; // sometimes type is empty
                        double lat = clientPartsMap["latitude"].toDouble();
                        double lng = clientPartsMap["longitude"].toDouble();
                        double alt = clientPartsMap["altitude"].toDouble();
                        CFrequency frequency = CFrequency(clientPartsMap["frequency"].toDouble(), CFrequencyUnit::MHz());
                        CCoordinateGeodetic position(lat, lng, -1);
                        CAltitude altitude(alt, CAltitude::MeanSeaLevel, CLengthUnit::ft());

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
                                    const QString w = QString("Illegal ICAO code in VATSIM data file: %1").arg(icaoCode);
                                    qWarning(w.toLatin1());
                                }
                            }

                            this->m_aircrafts.push_back(aircraft);
                        }
                        else if (clientType.startsWith('a'))
                        {
                            // ATC section
                            CLength range;
                            position.setHeight(altitude); // the altitude is elevation for a station
                            CAtcStation station(user.getCallsign().getStringAsSet(), user, frequency, position, range);
                            station.setOnline(true);
                            this->m_atcStations.push_back(station);
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
                            QDateTime dt = QDateTime::fromString(dts, "yyyyMMddHHmmss");
                            dt.setOffsetFromUtc(0);
                            if (dt == this->m_updateTimestamp) return; // still same data, terminate
                            this->m_updateTimestamp = dt;
                        }
                    }
                    break;
                case SectionVoiceServer:
                    {
                        QStringList voiceServerParts = currentLine.split(':');
                        if (voiceServerParts.size() < 3) continue;
                        BlackMisc::Network::CServer voiceServer(voiceServerParts.at(1), voiceServerParts.at(2), voiceServerParts.at(0), -1, CUser());
                        this->m_voiceServers.push_back(voiceServer);
                    }
                    break;
                case SectionNone:
                default:
                    break;
                }
            } // for each
        }
        emit this->dataRead();
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
} // namespace
