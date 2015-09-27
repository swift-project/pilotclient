/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "context_ownaircraft_impl.h"
#include "context_simulator.h"
#include "context_network.h"
#include "context_audio.h"
#include "context_application.h"
#include "context_runtime.h"
#include "context_settings.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    CContextOwnAircraft::CContextOwnAircraft(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        IContextOwnAircraft(mode, runtime),
        CIdentifiable(this)
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextSettings());
        this->setObjectName("CContextOwnAircraft");

        // Init own aircraft
        this->initOwnAircraft();
    }

    CContextOwnAircraft::~CContextOwnAircraft() { }

    CSimulatedAircraft CContextOwnAircraft::getOwnAircraft() const
    {
        if (this->m_debugEnabled) {CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        QReadLocker l(&m_lockAircraft);
        return this->m_ownAircraft;
    }

    CCoordinateGeodetic CContextOwnAircraft::getOwnAircraftPosition() const
    {
        QReadLocker l(&m_lockAircraft);
        return this->m_ownAircraft.getPosition();
    }

    CAircraftParts CContextOwnAircraft::getOwnAircraftParts() const
    {
        QReadLocker l(&m_lockAircraft);
        return this->m_ownAircraft.getParts();
    }

    CAircraftModel CContextOwnAircraft::getOwnAircraftModel() const
    {
        QReadLocker l(&m_lockAircraft);
        return this->m_ownAircraft.getModel();
    }

    CLength CContextOwnAircraft::getDistanceToOwnAircraft(const ICoordinateGeodetic &position) const
    {
        return getOwnAircraft().calculateGreatCircleDistance(position);
    }

    void CContextOwnAircraft::initOwnAircraft()
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextSettings());
        {
            QWriteLocker l(&m_lockAircraft);
            this->m_ownAircraft.initComSystems();
            this->m_ownAircraft.initTransponder();
            CAircraftSituation situation(
                CCoordinateGeodetic(
                    CLatitude::fromWgs84("N 049° 18' 17"),
                    CLongitude::fromWgs84("E 008° 27' 05"),
                    CLength(0, CLengthUnit::m())),
                CAltitude(312, CAltitude::MeanSeaLevel, CLengthUnit::ft())
            );
            this->m_ownAircraft.setSituation(situation);
            this->m_ownAircraft.setPilot(this->m_currentNetworkServer.get().getUser());

            // from simulator, if available
            this->m_ownAircraft.setCallsign(CCallsign("SWIFT")); // would come from settings

            //! \todo Own aircraft ICAO default data, this would need to come from somewhere (mappings) -> Own callsign, plane ICAO status, model used
            this->m_ownAircraft.setIcaoCodes(
                CAircraftIcaoCode("C172", "L1P"),
                CAirlineIcaoCode()
            );
        }

        // voice rooms, if network is already available
        if (this->getIContextNetwork())
        {
            this->resolveVoiceRooms();
        }
    }

    void CContextOwnAircraft::resolveVoiceRooms()
    {
        Q_ASSERT(this->getIContextAudio());
        Q_ASSERT(this->getIContextNetwork());
        Q_ASSERT(this->getIContextApplication());
        if (!this->getIContextNetwork() || !this->getIContextAudio() || !this->getIContextApplication()) { return; } // no chance to resolve rooms
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }

        if (this->m_voiceRoom1UrlOverride.isEmpty() && this->m_voiceRoom2UrlOverride.isEmpty() && !this->m_automaticVoiceRoomResolution) { return; }
        if (!this->m_automaticVoiceRoomResolution) { return; } // not responsible

        // requires correct frequencies set
        // but local network uses exactly this object here, so if frequencies are set here,
        // they are for network context as well
        CVoiceRoomList rooms = this->getIContextNetwork()->getSelectedVoiceRooms();

        if (!this->m_voiceRoom1UrlOverride.isEmpty()) rooms[0] = CVoiceRoom(this->m_voiceRoom1UrlOverride);
        if (!this->m_voiceRoom2UrlOverride.isEmpty()) rooms[1] = CVoiceRoom(this->m_voiceRoom2UrlOverride);

        // set the rooms
        emit this->getIContextApplication()->fakedSetComVoiceRoom(rooms);
    }

    bool CContextOwnAircraft::updateOwnModel(const CAircraftModel &model)
    {
        QWriteLocker l(&m_lockAircraft);
        bool changed = (this->m_ownAircraft.getModel() != model);
        if (!changed) { return false; }
        this->m_ownAircraft.setModel(model);
        return true;
    }

    bool CContextOwnAircraft::updateOwnSituation(const CAircraftSituation &situation)
    {
        QWriteLocker l(&m_lockAircraft);
        // there is intentionally no equal check
        this->m_ownAircraft.setSituation(situation);
        return true;
    }

    bool CContextOwnAircraft::updateOwnParts(const CAircraftParts &parts)
    {
        QWriteLocker l(&m_lockAircraft);
        bool changed = (this->m_ownAircraft.getParts() != parts);
        if (!changed) { return false; }
        this->m_ownAircraft.setParts(parts);
        return true;
    }

    bool CContextOwnAircraft::updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude)
    {
        if (this->m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << position << altitude; }
        QWriteLocker l(&m_lockAircraft);
        bool changed = (this->m_ownAircraft.getPosition() != position);
        if (changed) { this->m_ownAircraft.setPosition(position); }

        if (this->m_ownAircraft.getAltitude() != altitude)
        {
            changed = true;
            this->m_ownAircraft.setAltitude(altitude);
        }
        return changed;
    }

    bool CContextOwnAircraft::updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const CIdentifier &originator)
    {
        if (this->m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << com1 << com2 << transponder; }
        bool changed;
        {
            QWriteLocker l(&m_lockAircraft);
            changed = this->m_ownAircraft.hasChangedCockpitData(com1, com2, transponder);
            if (changed) { this->m_ownAircraft.setCockpit(com1, com2, transponder); }
        }
        if (changed)
        {
            emit this->changedAircraftCockpit(this->m_ownAircraft, originator);
            this->resolveVoiceRooms();
        }
        return changed;
    }

    bool CContextOwnAircraft::updateActiveComFrequency(const CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit unit, const CIdentifier &originator)
    {
        if (unit != CComSystem::Com1 && unit != CComSystem::Com2) { return false; }
        if (!CComSystem::isValidComFrequency(frequency)) { return false; }
        CComSystem com1, com2;
        CTransponder xpdr;
        {
            QReadLocker l(&m_lockAircraft);
            com1 = this->m_ownAircraft.getCom1System();
            com2 = this->m_ownAircraft.getCom2System();
            xpdr = this->m_ownAircraft.getTransponder();
        }
        if (unit == CComSystem::Com1)
        {
            com1.setFrequencyActive(frequency);
        }
        else
        {
            com2.setFrequencyActive(frequency);
        }
        return updateCockpit(com1, com2, xpdr, originator);
    }

    bool CContextOwnAircraft::updateOwnAircraftPilot(const CUser &pilot)
    {
        {
            QWriteLocker l(&m_lockAircraft);
            if (this->m_ownAircraft.getPilot() == pilot) { return false; }
            this->m_ownAircraft.setPilot(pilot);
        }
        emit changedPilot(pilot);
        return true;
    }

    bool CContextOwnAircraft::updateOwnCallsign(const CCallsign &callsign)
    {
        {
            QWriteLocker l(&m_lockAircraft);
            if (this->m_ownAircraft.getCallsign() == callsign) { return false; }
            this->m_ownAircraft.setCallsign(callsign);
        }
        emit changedCallsign(callsign);
        return true;
    }

    bool CContextOwnAircraft::updateOwnIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode)
    {
        {
            QWriteLocker l(&m_lockAircraft);
            return this->m_ownAircraft.setIcaoCodes(aircraftIcaoCode, airlineIcaoCode);
        }
        emit changedAircraftIcaoCodes(aircraftIcaoCode, airlineIcaoCode);
    }

    bool CContextOwnAircraft::updateSelcal(const CSelcal &selcal, const CIdentifier &originator)
    {
        {
            QWriteLocker l(&m_lockAircraft);
            if (this->m_ownAircraft.getSelcal() == selcal) { return false; }
            this->m_ownAircraft.setSelcal(selcal);
        }
        emit this->changedSelcal(selcal, originator);
        return true;
    }

    void CContextOwnAircraft::setAudioOutputVolume(int outputVolume)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << outputVolume; }
        if (this->getIContextAudio()) this->getIContextAudio()->setVoiceOutputVolume(outputVolume);
    }

    void CContextOwnAircraft::ps_changedAtcStationOnlineConnectionStatus(const CAtcStation &atcStation, bool connected)
    {
        // any of our active frequencies?
        Q_UNUSED(connected);
        CSimulatedAircraft myAircraft(getOwnAircraft());
        if (atcStation.getFrequency() != myAircraft.getCom1System().getFrequencyActive() && atcStation.getFrequency() != myAircraft.getCom2System().getFrequencyActive()) { return; }
        this->resolveVoiceRooms();
    }

    void CContextOwnAircraft::ps_changedSimulatorModel(const CSimulatedAircraft &ownAircraft)
    {
        CAircraftModel model(ownAircraft.getModel());
        QWriteLocker l(&m_lockAircraft);
        this->m_ownAircraft.setModel(model);
    }

    void CContextOwnAircraft::setAudioVoiceRoomOverrideUrls(const QString &voiceRoom1Url, const QString &voiceRoom2Url)
    {
        if (this->m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << voiceRoom1Url << voiceRoom2Url; }
        this->m_voiceRoom1UrlOverride = voiceRoom1Url.trimmed();
        this->m_voiceRoom2UrlOverride = voiceRoom2Url.trimmed();
        this->resolveVoiceRooms();
    }

    void CContextOwnAircraft::enableAutomaticVoiceRoomResolution(bool enable)
    {
        if (this->m_debugEnabled) {CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << enable; }
        this->m_automaticVoiceRoomResolution = enable;
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
