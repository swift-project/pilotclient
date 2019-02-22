/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/contextownaircraftimpl.h"

// ----- cross context -----
#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"
// ----- cross context -----

#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/audio/voiceroom.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackmisc/network/server.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/transponder.h"
#include "blackcore/db/databaseutils.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/compare.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/sequence.h"
#include "blackmisc/statusmessage.h"

#include <QReadLocker>
#include <QWriteLocker>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackCore::Db;

namespace BlackCore
{
    namespace Context
    {
        CContextOwnAircraft::CContextOwnAircraft(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) :
            IContextOwnAircraft(mode, runtime),
            CIdentifiable(this)
        {
            Q_ASSERT(this->getRuntime());

            connect(&m_historyTimer, &QTimer::timeout, this, &CContextOwnAircraft::evaluateUpdateHistory);
            this->setObjectName("CContextOwnAircraft");
            m_historyTimer.setObjectName(this->objectName() + "::historyTimer");
            m_historyTimer.start(2500);
            m_situationHistory.setSortHint(CAircraftSituationList::TimestampLatestFirst);

            CContextOwnAircraft::registerHelp();

            if (sApp && sApp->getWebDataServices())
            {
                connect(sApp->getWebDataServices(), &CWebDataServices::swiftDbAllDataRead, this, &CContextOwnAircraft::allSwiftWebDataRead);
            }

            // Init own aircraft
            this->initOwnAircraft();
        }

        CContextOwnAircraft::~CContextOwnAircraft() { }

        CContextOwnAircraft *CContextOwnAircraft::registerWithDBus(CDBusServer *server)
        {
            if (!server || m_mode != CCoreFacadeConfig::LocalInDBusServer) return this;
            server->addObject(IContextOwnAircraft::ObjectPath(), this);
            return this;
        }

        CSimulatedAircraft CContextOwnAircraft::getOwnAircraft() const
        {
            if (m_debugEnabled) {CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            QReadLocker l(&m_lockAircraft);
            return m_ownAircraft;
        }

        CComSystem CContextOwnAircraft::getOwnComSystem(CComSystem::ComUnit unit) const
        {
            if (m_debugEnabled) {CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            QReadLocker l(&m_lockAircraft);
            return m_ownAircraft.getComSystem(unit);
        }

        CTransponder CContextOwnAircraft::getOwnTransponder() const
        {
            if (m_debugEnabled) {CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            QReadLocker l(&m_lockAircraft);
            return m_ownAircraft.getTransponder();
        }

        CCoordinateGeodetic CContextOwnAircraft::getOwnAircraftPosition() const
        {
            QReadLocker l(&m_lockAircraft);
            return m_ownAircraft.getPosition();
        }

        CAircraftSituation CContextOwnAircraft::getOwnAircraftSituation() const
        {
            QReadLocker l(&m_lockAircraft);
            return m_ownAircraft.getSituation();
        }

        CAircraftParts CContextOwnAircraft::getOwnAircraftParts() const
        {
            QReadLocker l(&m_lockAircraft);
            return m_ownAircraft.getParts();
        }

        CAircraftModel CContextOwnAircraft::getOwnAircraftModel() const
        {
            QReadLocker l(&m_lockAircraft);
            return m_ownAircraft.getModel();
        }

        CLength CContextOwnAircraft::getDistanceToOwnAircraft(const ICoordinateGeodetic &position) const
        {
            return getOwnAircraft().calculateGreatCircleDistance(position);
        }

        void CContextOwnAircraft::initOwnAircraft()
        {
            Q_ASSERT(this->getRuntime());
            CSimulatedAircraft ownAircraft;
            {
                // use copy to minimize lock time
                QReadLocker rl(&m_lockAircraft);
                ownAircraft = m_ownAircraft;
            }

            ownAircraft.initComSystems();
            ownAircraft.initTransponder();
            ownAircraft.setSituation(getDefaultSituation());
            ownAircraft.setPilot(m_currentNetworkServer.get().getUser());

            // If we already have a model from somehwere, keep it, otherwise init default
            ownAircraft.setModel(this->reverseLookupModel(ownAircraft.getModel()));
            if (!ownAircraft.getAircraftIcaoCode().hasValidDesignator())
            {
                ownAircraft.setModel(getDefaultOwnAircraftModel());
            }

            // override empty values
            if (!ownAircraft.hasValidCallsign())
            {
                ownAircraft.setCallsign(CCallsign("SWIFT"));
            }

            // update object
            {
                QWriteLocker l(&m_lockAircraft);
                m_ownAircraft = ownAircraft;
            }

            // voice rooms, if network is already available
            if (this->getIContextNetwork())
            {
                this->resolveVoiceRooms(); // init own aircraft
            }
        }

        void CContextOwnAircraft::resolveVoiceRooms()
        {
            if (!this->getIContextNetwork() || !this->getIContextAudio() || !this->getIContextApplication()) { return; } // no chance to resolve rooms
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }

            if (m_voiceRoom1UrlOverride.isEmpty() && m_voiceRoom2UrlOverride.isEmpty() && !m_automaticVoiceRoomResolution) { return; }
            if (!m_automaticVoiceRoomResolution) { return; } // not responsible

            // requires correct frequencies set
            // but local network uses exactly this object here, so if frequencies are set here,
            // they are set for network context as well
            CVoiceRoomList rooms = this->getIContextNetwork()->getSelectedVoiceRooms();

            if (!m_voiceRoom1UrlOverride.isEmpty()) { rooms[0] = CVoiceRoom(m_voiceRoom1UrlOverride); }
            if (!m_voiceRoom2UrlOverride.isEmpty()) { rooms[1] = CVoiceRoom(m_voiceRoom2UrlOverride); }

            // set the rooms
            emit this->getIContextApplication()->fakedSetComVoiceRoom(rooms);
        }

        void CContextOwnAircraft::evaluateUpdateHistory()
        {
            if (!m_history) { return; }
            if (!this->getIContextSimulator()) { return; }

            if (this->getIContextSimulator()->isSimulatorSimulating())
            {
                if (!m_situationHistory.isEmpty())
                {
                    QReadLocker rl(&m_lockAircraft);
                    const CAircraftSituationList situations = m_situationHistory;
                    rl.unlock();

                    // using copy to minimize lock time
                    // 500km/h => 1sec: 0.1388 km
                    static const CLength maxDistance(25, CLengthUnit::km());
                    const bool jumpDetected = situations.containsObjectOutsideRange(situations.front(), maxDistance);

                    if (jumpDetected)
                    {
                        {
                            QWriteLocker wl(&m_lockAircraft);
                            m_situationHistory.clear();
                        }
                        emit this->movedAircraft();
                    }
                }
            } // only if simulating
        }

        CAircraftModel CContextOwnAircraft::reverseLookupModel(const CAircraftModel &model)
        {
            bool modified = false;
            const CAircraftModel reverseModel = CDatabaseUtils::consolidateOwnAircraftModelWithDbData(model, false, &modified);
            return reverseModel;
        }

        bool CContextOwnAircraft::updateOwnModel(const CAircraftModel &model)
        {
            return updateOwnModel(model, this->identifier());
        }

        bool CContextOwnAircraft::updateOwnModel(const CAircraftModel &model, const CIdentifier &identifier)
        {
            CAircraftModel updateModel(this->reverseLookupModel(model));
            {
                QWriteLocker l(&m_lockAircraft);
                const bool changed = (m_ownAircraft.getModel() != updateModel);
                if (!changed) { return false; }
                m_ownAircraft.setModel(updateModel);
            }

            // changed model
            emit this->ps_changedModel(updateModel, identifier);
            return true;
        }

        bool CContextOwnAircraft::updateOwnSituation(const CAircraftSituation &situation)
        {
            QWriteLocker l(&m_lockAircraft);
            // there is intentionally no equal check
            m_ownAircraft.setSituation(situation);

            if (m_situationHistory.isEmpty() || qAbs(situation.getTimeDifferenceMs(m_situationHistory.front())) > MinHistoryDeltaMs)
            {
                m_situationHistory.push_frontKeepLatestAdjustedFirst(situation, true);
                if (m_situationHistory.size() > MaxHistoryElements) { m_situationHistory.pop_back(); }
            }
            return true;
        }

        bool CContextOwnAircraft::updateOwnParts(const CAircraftParts &parts)
        {
            QWriteLocker l(&m_lockAircraft);
            bool changed = (m_ownAircraft.getParts() != parts);
            if (!changed) { return false; }
            m_ownAircraft.setParts(parts);
            return true;
        }

        bool CContextOwnAircraft::updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const CAltitude &pressureAltitude)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << position << altitude; }
            QWriteLocker l(&m_lockAircraft);
            bool changed = (m_ownAircraft.getPosition() != position);
            if (changed) { m_ownAircraft.setPosition(position); }

            if (m_ownAircraft.getAltitude() != altitude)
            {
                changed = true;
                m_ownAircraft.setAltitude(altitude);
            }

            if (m_ownAircraft.getPressureAltitude() != pressureAltitude)
            {
                changed = true;
                m_ownAircraft.setPressureAltitude(pressureAltitude);
            }
            return changed;
        }

        bool CContextOwnAircraft::updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const CTransponder &transponder, const CIdentifier &originator)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << com1 << com2 << transponder; }
            bool changed;
            {
                QWriteLocker l(&m_lockAircraft);
                changed = m_ownAircraft.hasChangedCockpitData(com1, com2, transponder);
                if (changed) { m_ownAircraft.setCockpit(com1, com2, transponder); }
            }
            if (changed)
            {
                emit this->changedAircraftCockpit(m_ownAircraft, originator);
                this->resolveVoiceRooms(); // cockpit COM changed
            }
            return changed;
        }

        bool CContextOwnAircraft::updateTransponderMode(const CTransponder::TransponderMode &transponderMode, const CIdentifier &originator)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << transponderMode; }
            bool changed;
            {
                QWriteLocker l(&m_lockAircraft);
                changed = m_ownAircraft.setTransponderMode(transponderMode);
            }
            if (changed)
            {
                emit this->changedAircraftCockpit(m_ownAircraft, originator);
            }
            return changed;
        }

        bool CContextOwnAircraft::updateActiveComFrequency(const CFrequency &frequency, CComSystem::ComUnit unit, const CIdentifier &originator)
        {
            if (unit != CComSystem::Com1 && unit != CComSystem::Com2) { return false; }
            if (!CComSystem::isValidComFrequency(frequency)) { return false; }
            CComSystem com1, com2;
            CTransponder xpdr;
            {
                QReadLocker l(&m_lockAircraft);
                com1 = m_ownAircraft.getCom1System();
                com2 = m_ownAircraft.getCom2System();
                xpdr = m_ownAircraft.getTransponder();
            }
            if (unit == CComSystem::Com1)
            {
                com1.setFrequencyActive(frequency);
            }
            else
            {
                com2.setFrequencyActive(frequency);
            }
            return this->updateCockpit(com1, com2, xpdr, originator);
        }

        bool CContextOwnAircraft::updateOwnAircraftPilot(const CUser &pilot)
        {
            {
                QWriteLocker l(&m_lockAircraft);
                if (m_ownAircraft.getPilot() == pilot) { return false; }
                m_ownAircraft.setPilot(pilot);
            }
            emit this->changedPilot(pilot);
            return true;
        }

        void CContextOwnAircraft::toggleTransponderMode()
        {
            CTransponder xpdr;
            CComSystem com1;
            CComSystem com2;
            {
                QReadLocker l(&m_lockAircraft);
                com1 = m_ownAircraft.getCom1System();
                com2 = m_ownAircraft.getCom2System();
                xpdr = m_ownAircraft.getTransponder();
            }
            xpdr.toggleTransponderMode();
            this->updateCockpit(com1, com2, xpdr, this->identifier());
        }

        bool CContextOwnAircraft::setTransponderMode(CTransponder::TransponderMode mode)
        {
            return this->updateTransponderMode(mode, this->identifier());
        }

        bool CContextOwnAircraft::updateOwnCallsign(const CCallsign &callsign)
        {
            {
                QWriteLocker l(&m_lockAircraft);
                if (m_ownAircraft.getCallsign() == callsign) { return false; }
                m_ownAircraft.setCallsign(callsign);
            }
            emit this->changedCallsign(callsign);
            return true;
        }

        bool CContextOwnAircraft::updateOwnIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode)
        {
            {
                QWriteLocker l(&m_lockAircraft);
                if (!m_ownAircraft.setIcaoCodes(aircraftIcaoCode, airlineIcaoCode)) { return false; }
            }
            emit this->changedAircraftIcaoCodes(aircraftIcaoCode, airlineIcaoCode);
            return true;
        }

        bool CContextOwnAircraft::updateSelcal(const CSelcal &selcal, const CIdentifier &originator)
        {
            {
                QWriteLocker l(&m_lockAircraft);
                if (m_ownAircraft.getSelcal() == selcal) { return false; }
                m_ownAircraft.setSelcal(selcal);
            }
            emit this->changedSelcal(selcal, originator);
            return true;
        }

        void CContextOwnAircraft::setAudioOutputVolume(int outputVolume)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << outputVolume; }
            if (this->getIContextAudio()) this->getIContextAudio()->setVoiceOutputVolume(outputVolume);
        }

        void CContextOwnAircraft::xCtxChangedAtcStationOnlineConnectionStatus(const CAtcStation &atcStation, bool connected)
        {
            // any of our active frequencies?
            Q_UNUSED(connected);
            const CSimulatedAircraft myAircraft(this->getOwnAircraft());

            // relevant frequency
            if (myAircraft.getCom1System().isActiveFrequencyWithin8_33kHzChannel(atcStation.getFrequency()) || myAircraft.getCom2System().isActiveFrequencyWithin8_33kHzChannel(atcStation.getFrequency()))
            {
                this->resolveVoiceRooms(); // online status changed
            }
        }

        void CContextOwnAircraft::xCtxChangedSimulatorModel(const CAircraftModel &model, const CIdentifier &identifier)
        {
            this->updateOwnModel(model, identifier);
        }

        void CContextOwnAircraft::xCtxChangedSimulatorStatus(int status)
        {
            const ISimulator::SimulatorStatus s = static_cast<ISimulator::SimulatorStatus>(status);
            if (ISimulator::isAnyConnectedStatus(s))
            {
                // connected
            }
            else
            {
                // disconnected
                QWriteLocker l(&m_lockAircraft);
                m_situationHistory.clear();
            }
        }

        void CContextOwnAircraft::actionToggleTransponder(bool keydown)
        {
            if (!keydown) { return; }
            this->toggleTransponderMode();
        }

        void CContextOwnAircraft::actionIdent(bool keydown)
        {
            if (this->getOwnTransponder().isInStandby()) { return; }
            const CTransponder::TransponderMode m = keydown ? CTransponder::StateIdent : CTransponder::ModeC;
            this->updateTransponderMode(m, this->identifier());
        }

        void CContextOwnAircraft::allSwiftWebDataRead()
        {
            // we should already have received a reverse lookup model
            // from the driver
        }

        void CContextOwnAircraft::setAudioVoiceRoomOverrideUrls(const QString &voiceRoom1Url, const QString &voiceRoom2Url)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << voiceRoom1Url << voiceRoom2Url; }
            m_voiceRoom1UrlOverride = voiceRoom1Url.trimmed();
            m_voiceRoom2UrlOverride = voiceRoom2Url.trimmed();
            this->resolveVoiceRooms(); // override
        }

        void CContextOwnAircraft::enableAutomaticVoiceRoomResolution(bool enable)
        {
            if (m_debugEnabled) {CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << enable; }
            m_automaticVoiceRoomResolution = enable;
        }

        bool CContextOwnAircraft::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
        {
            Q_UNUSED(originator);
            if (commandLine.isEmpty()) { return false; }
            CSimpleCommandParser parser(
            {
                ".x", ".xpdr",    // transponder
                ".com1", ".com2", // com1, com2 frequencies
                ".c1", ".c2",     // com1, com2 frequencies
                ".selcal"
            });
            parser.parse(commandLine);
            if (!parser.isKnownCommand()) { return false; }

            CSimulatedAircraft myAircraft(this->getOwnAircraft());
            if (parser.matchesCommand(".x", ".xpdr")  && parser.countParts() > 1)
            {
                CTransponder transponder = myAircraft.getTransponder();
                int xprCode = parser.toInt(1);
                if (CTransponder::isValidTransponderCode(xprCode))
                {
                    transponder.setTransponderCode(xprCode);
                    // todo RW: replace originator
                    this->updateCockpit(myAircraft.getCom1System(), myAircraft.getCom2System(), transponder, CIdentifier("commandline"));
                    return true;
                }
                else
                {
                    CTransponder::TransponderMode mode = CTransponder::modeFromString(parser.part(1));
                    transponder.setTransponderMode(mode);
                    // todo RW: replace originator
                    this->updateCockpit(myAircraft.getCom1System(), myAircraft.getCom2System(), transponder, CIdentifier("commandline"));
                    return true;
                }
            }
            else if (parser.commandStartsWith("com") || parser.commandStartsWith("c"))
            {
                CFrequency frequency(parser.toDouble(1), CFrequencyUnit::MHz());
                if (CComSystem::isValidComFrequency(frequency))
                {
                    CComSystem com1 = myAircraft.getCom1System();
                    CComSystem com2 = myAircraft.getCom2System();
                    if (parser.commandEndsWith("1"))
                    {
                        com1.setFrequencyActive(frequency);
                    }
                    else if (parser.commandEndsWith("2"))
                    {
                        com2.setFrequencyActive(frequency);
                    }
                    else
                    {
                        return false;
                    }
                    this->updateCockpit(com1, com2, myAircraft.getTransponder(), identifier());
                    return true;
                }
            }
            else if (parser.matchesCommand(".selcal"))
            {
                if (CSelcal::isValidCode(parser.part(1)))
                {
                    this->updateSelcal(parser.part(1), this->identifier());
                    return true;
                }
            }
            return false;
        }
    } // namespace
} // namespace
