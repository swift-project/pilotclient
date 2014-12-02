/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "context_ownaircraft_impl.h"
#include "context_network.h"
#include "context_audio.h"
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

namespace BlackCore
{

    /*
     * Init this context
     */
    CContextOwnAircraft::CContextOwnAircraft(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        IContextOwnAircraft(mode, runtime)
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextSettings());

        // 1. Init own aircraft
        this->initOwnAircraft();
    }

    /*
     * Cleanup
     */
    CContextOwnAircraft::~CContextOwnAircraft() { }

    /*
     * Init own aircraft
     */
    void CContextOwnAircraft::initOwnAircraft()
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextSettings());
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
        this->m_ownAircraft.setPilot(this->getIContextSettings()->getNetworkSettings().getCurrentTrafficNetworkServer().getUser());

        // TODO: This would need to come from somewhere (mappings)
        // Own callsign, plane ICAO status, model used
        this->m_ownAircraft.setCallsign(CCallsign("SWIFT"));
        this->m_ownAircraft.setIcaoInfo(CAircraftIcao("C172", "L1P", "GA", "GA", "0000ff"));

        // voice rooms
        this->resolveVoiceRooms();
    }

    /*
     * Resolve voice rooms
     */
    void CContextOwnAircraft::resolveVoiceRooms()
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        if (this->m_voiceRoom1UrlOverride.isEmpty() && this->m_voiceRoom2UrlOverride.isEmpty() && !this->m_automaticVoiceRoomResolution) { return; }
        if (!this->getIContextNetwork()) { return; } // no chance to resolve rooms
        if (!this->getIContextAudio())   { return; } // no place to set rooms
        if (!this->m_automaticVoiceRoomResolution) { return; } // not responsible

        // requires correct frequencies set
        // but local network uses exactly this object here, so if frequencies are set here,
        // they are for network context as well
        CVoiceRoomList rooms = this->getIContextNetwork()->getSelectedVoiceRooms();

        if (!this->m_voiceRoom1UrlOverride.isEmpty()) rooms[0] = CVoiceRoom(this->m_voiceRoom1UrlOverride);
        if (!this->m_voiceRoom2UrlOverride.isEmpty()) rooms[1] = CVoiceRoom(this->m_voiceRoom2UrlOverride);

        // set the rooms
        this->getIContextAudio()->setComVoiceRooms(rooms);
    }

    /*
     * Own Aircraft
     */
    void CContextOwnAircraft::updateOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << ownAircraft() << originator;

        // trigger the correct signals
        bool changedCockpit = this->updateOwnCockpit(aircraft.getCom1System(), aircraft.getCom2System(), aircraft.getTransponder(), originator);
        bool changedPosition = this->updateOwnPosition(aircraft.getPosition(), aircraft.getAltitude() , originator);
        bool changedSituation = this->updateOwnSituation(aircraft.getSituation(), originator);
        bool changed = changedCockpit || changedPosition || changedSituation;

        // new voice rooms, cockpit has changed
        if (changedCockpit) this->resolveVoiceRooms();

        // any change triggers a global updated aircraft signal
        // comparison is not to avoid setting the value, but avoid wrong signals
        if (changed || this->m_ownAircraft != aircraft)
        {
            emit this->changedAircraft(aircraft, originator);

            // now set value
            this->m_ownAircraft = aircraft;
        }
    }

    /*
     * Own position
     */
    bool CContextOwnAircraft::updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const QString &originator)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << position << altitude << originator;
        bool changed = (this->m_ownAircraft.getPosition() != position);
        if (changed) { this->m_ownAircraft.setPosition(position); }

        if (this->m_ownAircraft.getAltitude() != altitude)
        {
            changed = true;
            this->m_ownAircraft.setAltitude(altitude);
        }

        if (changed)
        {
            emit this->changedAircraftPosition(this->m_ownAircraft, originator);
            emit this->changedAircraft(this->m_ownAircraft, originator);
        }
        return changed;
    }

    /*
     * Update own situation
     */
    bool CContextOwnAircraft::updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation, const QString &originator)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << situation;
        bool changed = this->m_ownAircraft.getSituation() != situation;
        if (!changed) return changed;

        if (changed)
        {
            this->m_ownAircraft.setSituation(situation);
            emit this->changedAircraftSituation(this->m_ownAircraft, originator);
            emit this->changedAircraft(this->m_ownAircraft, originator);
        }
        return changed;
    }

    /*
     * Own cockpit data
     */
    bool CContextOwnAircraft::updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const QString &originator)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << com1 << com2 << transponder;
        bool changed = this->m_ownAircraft.hasChangedCockpitData(com1, com2, transponder);
        if (changed)
        {
            this->m_ownAircraft.setCockpit(com1, com2, transponder);
            emit this->changedAircraftCockpit(this->m_ownAircraft, originator);
            emit this->changedAircraft(this->m_ownAircraft, originator);
            this->resolveVoiceRooms();
        }
        return changed;
    }

    bool CContextOwnAircraft::updatePilot(const CUser &pilot, const QString &originator)
    {
        if (this->m_ownAircraft.getPilot() == pilot) { return false; }
        this->m_ownAircraft.setPilot(pilot);
        emit this->changedAircraft(this->m_ownAircraft, originator);
        return true;
    }

    bool CContextOwnAircraft::updateCallsign(const CCallsign &callsign, const QString &originator)
    {
        if (this->m_ownAircraft.getCallsign() == callsign) { return false; }
        this->m_ownAircraft.setCallsign(callsign);
        emit this->changedAircraft(this->m_ownAircraft, originator);
        return true;
    }

    bool CContextOwnAircraft::updateIcaoData(const CAircraftIcao &icaoData, const QString &originator)
    {
        if (this->m_ownAircraft.getIcaoInfo() == icaoData) { return false; }
        this->m_ownAircraft.setIcaoInfo(icaoData);
        emit this->changedAircraft(this->m_ownAircraft, originator);
        return true;
    }

    bool CContextOwnAircraft::updateSelcal(const CSelcal &selcal, const QString &originator)
    {
        if (this->m_ownAircraft.getSelcal() == selcal) { return false; }
        this->m_ownAircraft.setSelcal(selcal);
        emit this->changedSelcal(selcal, originator);
        return true;
    }

    void CContextOwnAircraft::setAudioOutputVolumes(int outputVolumeCom1, int outputVolumeCom2)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << outputVolumeCom1 << outputVolumeCom2;

        CComSystem com1 = this->m_ownAircraft.getCom1System();
        com1.setVolumeOutput(outputVolumeCom1);
        this->m_ownAircraft.setCom1System(com1);

        CComSystem com2 = this->m_ownAircraft.getCom2System();
        com2.setVolumeOutput(outputVolumeCom2);
        this->m_ownAircraft.setCom2System(com1);

        if (this->getIContextAudio()) this->getIContextAudio()->setVolumes(com1, com2);
    }

    /*
     * Tune in / out voice room
     */
    void CContextOwnAircraft::ps_changedAtcStationOnlineConnectionStatus(const CAtcStation &atcStation, bool connected)
    {
        // any of our active frequencies?
        Q_UNUSED(connected);
        if (atcStation.getFrequency() != this->m_ownAircraft.getCom1System().getFrequencyActive() &&
                atcStation.getFrequency() != this->m_ownAircraft.getCom2System().getFrequencyActive()) return;
        this->resolveVoiceRooms();
    }

    /*
     *  Voice room URLs
     */
    void CContextOwnAircraft::setAudioVoiceRoomOverrideUrls(const QString &voiceRoom1Url, const QString &voiceRoom2Url)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << voiceRoom1Url << voiceRoom2Url;

        this->m_voiceRoom1UrlOverride = voiceRoom1Url.trimmed();
        this->m_voiceRoom2UrlOverride = voiceRoom2Url.trimmed();
        this->resolveVoiceRooms();
    }

    /*
     *  Voice room resolution
     */
    void CContextOwnAircraft::enableAutomaticVoiceRoomResolution(bool enable)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << enable;
        this->m_automaticVoiceRoomResolution = enable;
    }

    /*
     * Own aircraft
     */
    CAircraft CContextOwnAircraft::getOwnAircraft() const
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << this->m_ownAircraft;
        return this->m_ownAircraft;
    }

    /*
     * Command line entered
     */
    bool CContextOwnAircraft::parseCommandLine(const QString &commandLine)
    {
        static CSimpleCommandParser parser(
        {
            ".x", ".xpdr",    // transponder
            ".com1", ".com2", // com1, com2 frequencies
            ".selcal"
        });
        if (commandLine.isEmpty()) return false;
        parser.parse(commandLine);
        if (!parser.isKnownCommand()) return false;

        CAircraft ownAircraft = this->getOwnAircraft();
        if (parser.matchesCommand(".x", ".xpdr")  && parser.countParts() > 1)
        {
            CTransponder transponder = ownAircraft.getTransponder();
            int xprCode = parser.toInt(1);
            if (CTransponder::isValidTransponderCode(xprCode))
            {
                transponder.setTransponderCode(xprCode);
                this->updateOwnCockpit(ownAircraft.getCom1System(), ownAircraft.getCom2System(), transponder, "commandline");
                return true;
            }
            else
            {
                CTransponder::TransponderMode mode = CTransponder::modeFromString(parser.part(1));
                transponder.setTransponderMode(mode);
                this->updateOwnCockpit(ownAircraft.getCom1System(), ownAircraft.getCom2System(), transponder, "commandline");
                return true;
            }
        }
        else if (parser.commandStartsWith("com"))
        {
            CFrequency frequency(parser.toDouble(1), CFrequencyUnit::MHz());
            if (CComSystem::isValidComFrequency(frequency))
            {
                CComSystem com1 = ownAircraft.getCom1System();
                CComSystem com2 = ownAircraft.getCom2System();
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
                this->updateOwnCockpit(com1, com2, ownAircraft.getTransponder(), "commandline");
                return true;
            }
        }
        else if (parser.matchesCommand(".selcal"))
        {
            if (CSelcal::isValidCode(parser.part(1)))
            {
                this->updateSelcal(parser.part(1), "commandline");
                return true;
            }
        }
        return false;
    }

} // namespace
