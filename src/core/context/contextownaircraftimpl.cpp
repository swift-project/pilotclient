// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/context/contextownaircraftimpl.h"

#include <QReadLocker>
#include <QWriteLocker>
#include <QtGlobal>

// ----- cross context -----
#include "core/context/contextapplication.h"
#include "core/context/contextaudio.h"
#include "core/context/contextsimulator.h"
// ----- cross context -----

#include "core/application.h"
#include "core/db/databaseutils.h"
#include "core/webdataservices.h"
#include "misc/audio/notificationsounds.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/altitude.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/transponder.h"
#include "misc/dbusserver.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"
#include "misc/logcategories.h"
#include "misc/logmessage.h"
#include "misc/mixin/mixincompare.h"
#include "misc/network/server.h"
#include "misc/pq/physicalquantity.h"
#include "misc/pq/units.h"
#include "misc/sequence.h"
#include "misc/simplecommandparser.h"
#include "misc/statusmessage.h"

using namespace swift::misc;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::geo;
using namespace swift::misc::audio;
using namespace swift::misc::simulation;
using namespace swift::core;
using namespace swift::core::db;

namespace swift::core::context
{
    CContextOwnAircraft::CContextOwnAircraft(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime)
        : IContextOwnAircraft(mode, runtime), CIdentifiable(this)
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
            connect(sApp->getWebDataServices(), &CWebDataServices::swiftDbAllDataRead, this,
                    &CContextOwnAircraft::allSwiftWebDataRead);
        }

        // Init own aircraft
        this->initOwnAircraft();
    }

    CContextOwnAircraft::~CContextOwnAircraft() {}

    void CContextOwnAircraft::registerWithDBus(CDBusServer *server)
    {
        if (!server || getMode() != CCoreFacadeConfig::LocalInDBusServer) { return; }
        server->addObject(IContextOwnAircraft::ObjectPath(), this);
    }

    CSimulatedAircraft CContextOwnAircraft::getOwnAircraft() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        QReadLocker l(&m_lockAircraft);
        return m_ownAircraft;
    }

    CComSystem CContextOwnAircraft::getOwnComSystem(CComSystem::ComUnit unit) const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        QReadLocker l(&m_lockAircraft);
        return m_ownAircraft.getComSystem(unit);
    }

    CTransponder CContextOwnAircraft::getOwnTransponder() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        QReadLocker l(&m_lockAircraft);
        return m_ownAircraft.getTransponder();
    }

    CCallsign CContextOwnAircraft::getOwnCallsign() const
    {
        QReadLocker l(&m_lockAircraft);
        return m_ownAircraft.getCallsign();
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
        if (!ownAircraft.hasValidCallsign()) { ownAircraft.setCallsign(CCallsign("SWIFT")); }

        // update object
        {
            QWriteLocker l(&m_lockAircraft);
            m_ownAircraft = ownAircraft;
        }
    }

    void CContextOwnAircraft::evaluateUpdateHistory()
    {
        if (!m_history) { return; }
        if (!this->getIContextSimulator()) { return; }
        if (!this->getIContextSimulator()->isSimulatorSimulating()) { return; }

        QReadLocker rl(&m_lockAircraft);
        const CAircraftSituationList situations = m_situationHistory; // latest first
        rl.unlock();

        constexpr int minElements = qRound(MaxHistoryElements * 0.75);
        if (m_situationHistory.size() < minElements) { return; }

        // using copy to minimize lock time
        // 500km/h => 1sec: 0.1388 km
        // we check if there are situation for own aircraft outside the max.distance
        static const CLength maxDistance(25, CLengthUnit::km());
        const CAircraftSituation latest = situations.front();
        const bool jumpDetected = situations.containsObjectOutsideRange(situations.front(), maxDistance);

        if (jumpDetected)
        {
            const CAircraftSituationList ownDistances = situations.findFarthest(1, latest);
            const CLength distance = ownDistances.front().calculateGreatCircleDistance(latest);
            {
                QWriteLocker wl(&m_lockAircraft);
                m_situationHistory.clear();
            }
            emit this->movedAircraft(distance);
        }
    }

    CAircraftModel CContextOwnAircraft::reverseLookupModel(const CAircraftModel &model)
    {
        bool modified = false;
        const CAircraftModel reverseModel =
            CDatabaseUtils::consolidateOwnAircraftModelWithDbData(model, false, &modified);
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

        if (m_situationHistory.isEmpty() ||
            qAbs(situation.getTimeDifferenceMs(m_situationHistory.front())) > MinHistoryDeltaMs)
        {
            m_situationHistory.push_frontKeepLatestAdjustedFirst(situation, true);
            if (m_situationHistory.size() > MaxHistoryElements) { m_situationHistory.pop_back(); }
        }
        return true;
    }

    bool CContextOwnAircraft::updateOwnParts(const CAircraftParts &parts)
    {
        QWriteLocker l(&m_lockAircraft);
        const bool changed = (m_ownAircraft.getParts() != parts);
        if (!changed) { return false; }
        m_ownAircraft.setParts(parts);
        return true;
    }

    bool CContextOwnAircraft::updateOwnCG(const CLength &cg)
    {
        QWriteLocker l(&m_lockAircraft);
        const bool changed = (m_ownAircraft.getModel().getCG() != cg);
        if (!changed) { return false; }
        m_ownAircraft.setCG(cg);
        return true;
    }

    bool CContextOwnAircraft::updateOwnPosition(const swift::misc::geo::CCoordinateGeodetic &position,
                                                const swift::misc::aviation::CAltitude &altitude,
                                                const CAltitude &pressureAltitude)
    {
        if (isDebugEnabled())
        {
            CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << position << altitude;
        }
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

    bool CContextOwnAircraft::updateCockpit(const CComSystem &com1, const CComSystem &com2,
                                            const CTransponder &transponder, const CIdentifier &originator)
    {
        if (isDebugEnabled())
        {
            CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << com1 << com2 << transponder;
        }
        bool changed;
        {
            QWriteLocker l(&m_lockAircraft);
            changed = m_ownAircraft.hasChangedCockpitData(com1, com2, transponder);
            if (changed) { m_ownAircraft.setCockpit(com1, com2, transponder); }
        }
        if (changed) { emit this->changedAircraftCockpit(m_ownAircraft, originator); }
        return changed;
    }

    bool CContextOwnAircraft::updateTransponderMode(const CTransponder::TransponderMode &transponderMode,
                                                    const CIdentifier &originator)
    {
        if (isDebugEnabled())
        {
            CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << transponderMode;
        }
        bool changed;
        {
            QWriteLocker l(&m_lockAircraft);
            changed = m_ownAircraft.setTransponderMode(transponderMode);
        }
        if (changed) { emit this->changedAircraftCockpit(m_ownAircraft, originator); }
        return changed;
    }

    bool CContextOwnAircraft::updateActiveComFrequency(const CFrequency &frequency, CComSystem::ComUnit unit,
                                                       const CIdentifier &originator)
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
        if (unit == CComSystem::Com1) { com1.setFrequencyActive(frequency); }
        else { com2.setFrequencyActive(frequency); }

        const bool changed = this->updateCockpit(com1, com2, xpdr, originator);
        return changed;
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

    bool CContextOwnAircraft::updateOwnIcaoCodes(const swift::misc::aviation::CAircraftIcaoCode &aircraftIcaoCode,
                                                 const swift::misc::aviation::CAirlineIcaoCode &airlineIcaoCode)
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

    bool CContextOwnAircraft::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
    {
        Q_UNUSED(originator)
        if (commandLine.isEmpty()) { return false; }
        CSimpleCommandParser parser({ ".x", ".xpdr", // transponder
                                      ".com1", ".com2", // com1, com2 frequencies
                                      ".c1", ".c2", // com1, com2 frequencies
                                      ".selcal" });
        parser.parse(commandLine);
        if (!parser.isKnownCommand()) { return false; }

        CSimulatedAircraft myAircraft(this->getOwnAircraft());
        if (parser.matchesCommand(".x", ".xpdr") && parser.countParts() > 1)
        {
            CTransponder transponder = myAircraft.getTransponder();
            int xprCode = parser.toInt(1);
            if (CTransponder::isValidTransponderCode(xprCode))
            {
                transponder.setTransponderCode(xprCode);
                // todo RW: replace originator
                this->updateCockpit(myAircraft.getCom1System(), myAircraft.getCom2System(), transponder,
                                    CIdentifier("commandline"));
                return true;
            }
            else
            {
                CTransponder::TransponderMode mode = CTransponder::modeFromString(parser.part(1));
                transponder.setTransponderMode(mode);
                // todo RW: replace originator
                this->updateCockpit(myAircraft.getCom1System(), myAircraft.getCom2System(), transponder,
                                    CIdentifier("commandline"));
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
                if (parser.commandEndsWith("1")) { com1.setFrequencyActive(frequency); }
                else if (parser.commandEndsWith("2")) { com2.setFrequencyActive(frequency); }
                else { return false; }
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
} // namespace swift::core::context
