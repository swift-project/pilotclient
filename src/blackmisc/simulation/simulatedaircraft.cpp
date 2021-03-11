/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/verify.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/stringutils.h"
#include <QStringBuilder>
#include <tuple>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {
        CSimulatedAircraft::CSimulatedAircraft()
        {
            this->init();
        }

        CSimulatedAircraft::CSimulatedAircraft(const CAircraftModel &model) : m_models({model, model})
        {
            this->setCallsign(model.getCallsign());
            this->init();
        }

        CSimulatedAircraft::CSimulatedAircraft(const CCallsign &callsign, const CUser &user, const CAircraftSituation &situation) :
            m_callsign(callsign), m_pilot(user), m_situation(situation)
        {
            this->init();
        }

        CSimulatedAircraft::CSimulatedAircraft(const CCallsign &callsign, const CAircraftModel &model, const CUser &user, const CAircraftSituation &situation) :
            m_callsign(callsign), m_pilot(user), m_situation(situation)
        {
            this->setModel(model);
            this->init();
        }

        void CSimulatedAircraft::init()
        {
            Q_ASSERT_X(m_models.size() == 2, Q_FUNC_INFO, "Wrong model size");

            // sync some values, order here is crucial
            // set get/set thing here updates the redundant data (e.g. livery / model.livery)
            this->setCallsign(this->getCallsign());
            this->setIcaoCodes(this->getAircraftIcaoCode(), this->getAirlineIcaoCode());
            this->setModel(this->getModel());
        }

        void CSimulatedAircraft::setCockpit(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder)
        {
            this->setCom1System(com1);
            this->setCom2System(com2);
            this->setTransponder(transponder);
        }

        void CSimulatedAircraft::setCockpit(const CComSystem &com1, const CComSystem &com2, int transponderCode, CTransponder::TransponderMode transponderMode)
        {
            this->setCom1System(com1);
            this->setCom2System(com2);
            m_transponder.setTransponderCode(transponderCode);
            m_transponder.setTransponderMode(transponderMode);
        }

        bool CSimulatedAircraft::hasChangedCockpitData(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder) const
        {
            return this->getCom1System() != com1 || this->getCom2System() != com2 || this->getTransponder() != transponder;
        }

        bool CSimulatedAircraft::hasSameComData(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder)
        {
            return this->getCom1System() == com1 && this->getCom2System() == com2 && this->getTransponder() == transponder;
        }

        bool CSimulatedAircraft::isValidForLogin() const
        {
            if (m_callsign.isEmpty()) { return false; }
            if (m_pilot.isNull()) { return false; }
            return true;
        }

        void CSimulatedAircraft::setSituation(const CAircraftSituation &situation)
        {
            CAircraftVelocity velocity = m_situation.getVelocity();
            m_situation = situation;
            if (m_situation.getVelocity() == CAircraftVelocity{})
            {
                m_situation.setVelocity(velocity);
            }

            m_situation.setCallsign(this->getCallsign());
            this->setSupportingGndFlag(situation.hasInboundGroundDetails());
        }

        const CAircraftIcaoCode &CSimulatedAircraft::getAircraftIcaoCode() const
        {
            return m_models[CurrentModel].getAircraftIcaoCode();
        }

        void CSimulatedAircraft::setPilot(const Network::CUser &user)
        {
            m_pilot = user;
            m_pilot.setCallsign(m_callsign);
        }

        bool CSimulatedAircraft::isEnabled() const
        {
            if (!this->hasModelString()) { return false; }
            return m_enabled;
        }

        bool CSimulatedAircraft::setEnabled(bool enabled)
        {
            if (m_enabled == enabled) { return false; }
            m_enabled = enabled;
            return true;
        }

        bool CSimulatedAircraft::setFastPositionUpdates(bool useFastPositions)
        {
            if (m_fastPositionUpdates == useFastPositions) { return false; }
            m_fastPositionUpdates = useFastPositions;
            return true;
        }

        bool CSimulatedAircraft::toggleFastPositionUpdates()
        {
            return this->setFastPositionUpdates(!this->fastPositionUpdates());
        }

        bool CSimulatedAircraft::setRendered(bool rendered)
        {
            if (m_rendered == rendered) { return false; }
            m_rendered = rendered;
            return true;
        }

        const QString &CSimulatedAircraft::getAircraftIcaoCodeDesignator() const
        {
            return getAircraftIcaoCode().getDesignator();
        }

        QString CSimulatedAircraft::getAirlineAndAircraftIcaoCodeDesignators() const
        {
            if (this->hasAircraftAndAirlineDesignator()) { return this->getAircraftIcaoCodeDesignator() % u'/' % this->getAirlineIcaoCodeDesignator(); }
            if (this->hasAirlineDesignator()) { return this->getAirlineIcaoCodeDesignator(); }
            return this->getAircraftIcaoCodeDesignator();
        }

        const QString &CSimulatedAircraft::getAircraftIcaoCombinedType() const
        {
            return getAircraftIcaoCode().getCombinedType();
        }

        bool CSimulatedAircraft::setIcaoCodes(const CAircraftIcaoCode &aircraftIcaoCode, const CAirlineIcaoCode &airlineIcaoCode)
        {
            if (this->getLivery().getAirlineIcaoCode() != airlineIcaoCode)
            {
                // create a dummy livery for given ICAO code
                const CLivery newLivery(CLivery::getStandardCode(airlineIcaoCode), airlineIcaoCode, "Standard auto generated");
                m_models[CurrentModel].setLivery(newLivery);
            }
            return m_models[CurrentModel].setAircraftIcaoCode(aircraftIcaoCode);
        }

        const CAirlineIcaoCode &CSimulatedAircraft::getAirlineIcaoCode() const
        {
            return m_models[CurrentModel].getAirlineIcaoCode();
        }

        const QString &CSimulatedAircraft::getAirlineIcaoCodeDesignator() const
        {
            return this->getAirlineIcaoCode().getDesignator();
        }

        void CSimulatedAircraft::setAircraftIcaoDesignator(const QString &designator)
        {
            m_models[CurrentModel].setAircraftIcaoDesignator(designator);
        }

        bool CSimulatedAircraft::hasAircraftDesignator() const
        {
            return this->getAircraftIcaoCode().hasDesignator();
        }

        bool CSimulatedAircraft::hasAircraftAndAirlineDesignator() const
        {
            return this->getModel().hasAircraftAndAirlineDesignator();
        }

        const CComSystem CSimulatedAircraft::getComSystem(CComSystem::ComUnit unit) const
        {
            switch (unit)
            {
            case CComSystem::Com1: return this->getCom1System();
            case CComSystem::Com2: return this->getCom2System();
            default: break;
            }
            BLACK_VERIFY_X(false, Q_FUNC_INFO, "Wrong unit");
            return CComSystem(); // avoid warning
        }

        void CSimulatedAircraft::setCockpit(const CSimulatedAircraft &aircraft)
        {
            this->setCockpit(aircraft.getCom1System(), aircraft.getCom2System(), aircraft.getTransponder());
            this->setSelcal(aircraft.getSelcal());
        }

        void CSimulatedAircraft::setComSystem(const CComSystem &com, CComSystem::ComUnit unit)
        {
            switch (unit)
            {
            case CComSystem::Com1: this->setCom1System(com); break;
            case CComSystem::Com2: this->setCom2System(com); break;
            default: BLACK_VERIFY_X(false, Q_FUNC_INFO, "Wrong unit"); break;
            }
        }

        bool CSimulatedAircraft::setCom1ActiveFrequency(const CFrequency &frequency)
        {
            if (!CComSystem::isValidComFrequency(frequency)) { return false; }
            m_com1system.setFrequencyActive(frequency);
            return true;
        }

        bool CSimulatedAircraft::setCom2ActiveFrequency(const CFrequency &frequency)
        {
            if (!CComSystem::isValidComFrequency(frequency)) { return false; }
            m_com2system.setFrequencyActive(frequency);
            return true;
        }

        bool CSimulatedAircraft::setComActiveFrequency(const CFrequency &frequency, CComSystem::ComUnit unit)
        {
            if (!CComSystem::isValidComFrequency(frequency)) { return false; }
            switch (unit)
            {
            case CComSystem::Com1: return this->setCom1ActiveFrequency(frequency);
            case CComSystem::Com2: return this->setCom2ActiveFrequency(frequency);
            default: BLACK_VERIFY_X(false, Q_FUNC_INFO, "Wrong unit"); break;
            }
            return false;
        }

        void CSimulatedAircraft::initComSystems()
        {
            CComSystem com1("COM1", CPhysicalQuantitiesConstants::FrequencyUnicom(), CPhysicalQuantitiesConstants::FrequencyUnicom());
            CComSystem com2("COM2", CPhysicalQuantitiesConstants::FrequencyUnicom(), CPhysicalQuantitiesConstants::FrequencyUnicom());
            this->setCom1System(com1);
            this->setCom2System(com2);
        }

        void CSimulatedAircraft::initTransponder()
        {
            const CTransponder xpdr(7000, CTransponder::StateStandby);
            this->setTransponder(xpdr);
        }

        int CSimulatedAircraft::getEnginesCount() const
        {
            const int engines = this->getModel().getAircraftIcaoCode().getEnginesCount();
            return engines >= 0 ? engines : m_parts.getEnginesCount();
        }

        CAircraftLights CSimulatedAircraft::getLights() const
        {
            return m_parts.getLights();
        }

        void CSimulatedAircraft::setParts(const CAircraftParts &parts)
        {
            m_parts = parts;
        }

        void CSimulatedAircraft::setLights(CAircraftLights &lights)
        {
            m_parts.setLights(lights);
        }

        void CSimulatedAircraft::setAllLightsOn()
        {
            m_parts.setAllLightsOn();
        }

        void CSimulatedAircraft::setAllLightsOff()
        {
            m_parts.setAllLightsOff();
        }

        bool CSimulatedAircraft::isVtol() const
        {
            return this->getModel().isVtol();
        }

        bool CSimulatedAircraft::isMilitary() const
        {
            return this->getModel().isMilitary();
        }

        QString CSimulatedAircraft::getCombinedIcaoLiveryString(bool networkModel) const
        {
            const CAircraftModel model(networkModel ? this->getNetworkModel() : this->getModel());
            if (model.hasAircraftAndAirlineDesignator())
            {
                if (model.getLivery().hasCombinedCode())
                {
                    static const QString s("%1 (%2 %3)");
                    return s.arg(model.getAircraftIcaoCodeDesignator(), model.getAirlineIcaoCodeDesignator(), model.getLivery().getCombinedCode());
                }
                else
                {
                    static const QString s("%1 (%2)");
                    return s.arg(model.getAircraftIcaoCodeDesignator(), model.getAirlineIcaoCodeDesignator());
                }
            }

            if (!this->hasAircraftDesignator())
            {
                return model.getLivery().getCombinedCode();
            }
            else if (model.getLivery().hasCombinedCode())
            {
                static const QString s("%1 (%2)");
                return s.arg(model.getAircraftIcaoCodeDesignator(), model.getLivery().getCombinedCode());
            }

            return model.getAircraftIcaoCode().getDesignator();
        }

        QVariant CSimulatedAircraft::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
        {
            if (index.isMyself()) { return QVariant::fromValue(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModel: return this->getModel().propertyByIndex(index.copyFrontRemoved());
            case IndexNetworkModel: return this->getNetworkModel().propertyByIndex(index.copyFrontRemoved());
            case IndexNetworkModelAircraftIcaoDifference: return this->getNetworkModelAircraftIcaoDifference();
            case IndexNetworkModelAirlineIcaoDifference: return this->getNetworkModelAirlineIcaoDifference();
            case IndexNetworkModelLiveryDifference: return this->getNetworkModelLiveryDifference();
            case IndexEnabled:  return QVariant::fromValue(this->isEnabled());
            case IndexRendered: return QVariant::fromValue(this->isRendered());
            case IndexPartsSynchronized: return QVariant::fromValue(this->isPartsSynchronized());
            case IndexFastPositionUpdates: return QVariant::fromValue(this->fastPositionUpdates());
            case IndexSupportsGndFlag: return QVariant::fromValue(this->isSupportingGndFlag());
            case IndexCallsign: return m_callsign.propertyByIndex(index.copyFrontRemoved());
            case IndexPilot:    return m_pilot.propertyByIndex(index.copyFrontRemoved());
            case IndexRelativeDistance: return m_relativeDistance.propertyByIndex(index.copyFrontRemoved());
            case IndexCom1System:  return m_com1system.propertyByIndex(index.copyFrontRemoved());
            case IndexCom2System:  return m_com2system.propertyByIndex(index.copyFrontRemoved());
            case IndexTransponder: return m_transponder.propertyByIndex(index.copyFrontRemoved());
            case IndexSituation: return m_situation.propertyByIndex(index.copyFrontRemoved());
            case IndexAircraftIcaoCode: return this->getAircraftIcaoCode().propertyByIndex(index.copyFrontRemoved());
            case IndexLivery: return this->getLivery().propertyByIndex(index.copyFrontRemoved());
            case IndexParts: return m_parts.propertyByIndex(index.copyFrontRemoved());
            case IndexIsVtol: return QVariant::fromValue(this->isVtol());
            case IndexCombinedIcaoLiveryString: return QVariant::fromValue(this->getCombinedIcaoLiveryString(false));
            case IndexCombinedIcaoLiveryStringNetworkModel: return QVariant::fromValue(this->getCombinedIcaoLiveryString(true));
            default:
                return (ICoordinateWithRelativePosition::canHandleIndex(index)) ?
                       ICoordinateWithRelativePosition::propertyByIndex(index) :
                       CValueObject::propertyByIndex(index);
            }
        }

        void CSimulatedAircraft::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.value<CSimulatedAircraft>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsign:     m_callsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexPilot:        m_pilot.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexRelativeDistance: m_relativeDistance.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexCom1System:   m_com1system.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexCom2System:   m_com2system.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexTransponder:  m_transponder.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexSituation:    m_situation.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexParts:        m_parts.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexNetworkModel: m_models[NetworkModel].setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexEnabled:      m_enabled = variant.toBool(); break;
            case IndexRendered:     m_rendered = variant.toBool(); break;
            case IndexPartsSynchronized:   m_partsSynchronized = variant.toBool(); break;
            case IndexFastPositionUpdates: m_fastPositionUpdates = variant.toBool(); break;
            case IndexSupportsGndFlag:     m_supportsGndFlag = variant.toBool(); break;
            case IndexLivery: Q_ASSERT_X(false, Q_FUNC_INFO, "Unsupported"); break;
            case IndexModel:
                m_models[CurrentModel].setPropertyByIndex(index.copyFrontRemoved(), variant);
                this->setModel(m_models[CurrentModel]); // sync some values such as callsign
                break;
            default:
                if (ICoordinateWithRelativePosition::canHandleIndex(index))
                {
                    ICoordinateWithRelativePosition::setPropertyByIndex(index, variant);
                }
                else
                {
                    CValueObject::setPropertyByIndex(index, variant);
                }
                break;
            }
        }

        int CSimulatedAircraft::comparePropertyByIndex(CPropertyIndexRef index, const CSimulatedAircraft &compareValue) const
        {
            if (index.isMyself()) { return m_callsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign()); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsign:  return m_callsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
            case IndexPilot:     return m_pilot.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getPilot());
            case IndexSituation: return m_situation.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getSituation());
            case IndexRelativeDistance: return m_relativeDistance.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getRelativeDistance());
            case IndexCom1System:  return m_com1system.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCom1System());
            case IndexCom2System:  return m_com2system.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCom2System());
            case IndexTransponder: return Compare::compare(m_transponder.getTransponderCode(), compareValue.getTransponder().getTransponderCode());
            case IndexLivery: return this->getLivery().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getLivery());
            case IndexParts:  return m_parts.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getParts());
            case IndexModel:  return m_models[CurrentModel].comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getModel());
            case IndexNetworkModel: return m_models[NetworkModel].comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getModel());
            case IndexNetworkModelAircraftIcaoDifference: return this->getNetworkModelAircraftIcaoDifference().compare(compareValue.getNetworkModelAircraftIcaoDifference());
            case IndexNetworkModelAirlineIcaoDifference:  return this->getNetworkModelAirlineIcaoDifference().compare(compareValue.getNetworkModelAirlineIcaoDifference());
            case IndexNetworkModelLiveryDifference:       return this->getNetworkModelLiveryDifference().compare(compareValue.getNetworkModelLiveryDifference());
            case IndexEnabled:             return Compare::compare(this->isEnabled(),  compareValue.isEnabled());
            case IndexRendered:            return Compare::compare(this->isRendered(), compareValue.isRendered());
            case IndexPartsSynchronized:   return Compare::compare(this->isPartsSynchronized(), compareValue.isPartsSynchronized());
            case IndexFastPositionUpdates: return Compare::compare(this->fastPositionUpdates(), compareValue.fastPositionUpdates());
            case IndexSupportsGndFlag:     return Compare::compare(this->isSupportingGndFlag(), compareValue.isSupportingGndFlag());
            case IndexCombinedIcaoLiveryString: return this->getCombinedIcaoLiveryString(false).compare(compareValue.getCombinedIcaoLiveryString(false));
            case IndexCombinedIcaoLiveryStringNetworkModel: return this->getCombinedIcaoLiveryString(true).compare(compareValue.getCombinedIcaoLiveryString(true));
            default:
                if (ICoordinateWithRelativePosition::canHandleIndex(index))
                {
                    return ICoordinateWithRelativePosition::comparePropertyByIndex(index, compareValue);
                }
                break;
            }
            BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable("No comparison for index " + index.toQString()));
            return 0;
        }

        const CAircraftModel &CSimulatedAircraft::getNetworkModelOrModel() const
        {
            Q_ASSERT_X(m_models.size() == 2, Q_FUNC_INFO, "Wrong model size");
            return this->hasNetworkModel() ? m_models[NetworkModel] : m_models[CurrentModel];
        }

        bool CSimulatedAircraft::hasNetworkModel() const
        {
            Q_ASSERT_X(m_models.size() == 2, Q_FUNC_INFO, "Wrong model size");
            return m_models[NetworkModel].hasModelString() || !m_models[NetworkModel].getCallsign().isEmpty();
        }

        QString CSimulatedAircraft::getNetworkModelAircraftIcaoDifference() const
        {
            const CAircraftIcaoCode icaoNw(this->getNetworkModel().getAircraftIcaoCode());
            if (!icaoNw.hasDesignator()) { return QStringLiteral("[x] no nw. ICAO"); }

            const CAircraftIcaoCode icaoModel(this->getModel().getAircraftIcaoCode());
            if (!icaoModel.hasDesignator())   { return QStringLiteral("[x] no sim. ICAO"); }

            if (icaoModel.isDbEqual(icaoNw) || icaoModel == icaoNw)  { return u"[==] " % icaoModel.getDesignatorDbKey(); }
            if (icaoModel.getDesignator() == icaoNw.getDesignator()) { return u"[=] "  % icaoModel.getDesignator(); }
            return QStringLiteral("%1 -> %2").arg(icaoNw.getDesignator(), icaoModel.getDesignator());
        }

        QString CSimulatedAircraft::getNetworkModelAirlineIcaoDifference() const
        {
            static const QString diff("%1 -> %2");

            if (this->getModel().getLivery().isColorLivery() || this->getNetworkModel().getLivery().isColorLivery())
            {
                if (this->getModel().getLivery().isColorLivery() && this->getNetworkModel().getLivery().isColorLivery())
                {
                    return ("col/col");
                }

                if (this->getModel().getLivery().isColorLivery())
                {
                    return diff.arg("col", this->getNetworkModel().getAirlineIcaoCode().getDesignator());
                }
                return diff.arg(this->getNetworkModel().getAirlineIcaoCode().getDesignator(), "col");
            }

            const CAirlineIcaoCode icao(this->getModel().getAirlineIcaoCode());
            const CAirlineIcaoCode icaoNw(this->getNetworkModel().getAirlineIcaoCode());
            if (icao.isDbEqual(icaoNw) || icao == icaoNw) { return u"[==] " % icao.getDesignatorDbKey(); }
            if (icao.getDesignator() == icaoNw.getDesignator()) { return u"[=] " % icao.getDesignatorDbKey(); }
            return diff.arg(icaoNw.getDesignator(), icao.getDesignator());
        }

        QString CSimulatedAircraft::getNetworkModelLiveryDifference() const
        {
            Q_ASSERT_X(m_models.size() == 2, Q_FUNC_INFO, "Wrong model size");

            const CLivery livery(this->getModel().getLivery());
            const CLivery liveryNw(this->getNetworkModel().getLivery());
            if (livery.isDbEqual(liveryNw) || livery == liveryNw) { return QStringLiteral("[==] ") + livery.getCombinedCodePlusInfo(); }
            if (livery.getCombinedCode() == liveryNw.getCombinedCode()) { return QStringLiteral("[=] ") + livery.getCombinedCodePlusInfo(); }
            if (livery.isAirlineLivery() && liveryNw.isAirlineLivery()) { return this->getNetworkModelAirlineIcaoDifference(); }

            static const QString diff("%1 -> %2");
            return diff.arg(liveryNw.getCombinedCodePlusInfo(), livery.getCombinedCodePlusInfo());
        }

        void CSimulatedAircraft::setModel(const CAircraftModel &model)
        {
            Q_ASSERT_X(m_models.size() == 2, Q_FUNC_INFO, "Wrong model size");

            // sync the callsigns
            m_models[CurrentModel] = model;
            this->setCallsign(this->hasValidCallsign() ? this->getCallsign() : model.getCallsign());
            this->setIcaoCodes(model.getAircraftIcaoCode(), model.getAirlineIcaoCode());
        }

        void CSimulatedAircraft::setNetworkModel(const CAircraftModel &model)
        {
            Q_ASSERT_X(m_models.size() == 2, Q_FUNC_INFO, "Wrong model size");
            m_models[NetworkModel] = model;
        }

        bool CSimulatedAircraft::resetToNetworkModel()
        {
            Q_ASSERT_X(m_models.size() == 2, Q_FUNC_INFO, "Wrong model size");
            const CAircraftModel nwModel = m_models[NetworkModel];
            m_models[CurrentModel] = nwModel;
            return true;
        }

        bool CSimulatedAircraft::setCG(const CLength &cg)
        {
            if (cg.isNull()) { return false; }
            // ???? set to both models, or only the matched model ????
            const int c = m_models.setCG(cg);
            return c > 0;
        }

        void CSimulatedAircraft::setModelString(const QString &modelString)
        {
            Q_ASSERT_X(m_models.size() == 2, Q_FUNC_INFO, "Wrong model size");
            m_models[CurrentModel].setModelString(modelString);
        }

        void CSimulatedAircraft::setCallsign(const CCallsign &callsign)
        {
            Q_ASSERT_X(m_models.size() == 2, Q_FUNC_INFO, "Wrong model size");
            m_callsign = callsign;
            m_models[CurrentModel].setCallsign(callsign);
            m_models[NetworkModel].setCallsign(callsign);
            m_pilot.setCallsign(callsign);
        }

        bool CSimulatedAircraft::isActiveFrequencyWithin8_33kHzChannel(const CFrequency &comFrequency) const
        {
            return m_com1system.isActiveFrequencyWithin8_33kHzChannel(comFrequency) ||
                   m_com2system.isActiveFrequencyWithin8_33kHzChannel(comFrequency);
        }

        bool CSimulatedAircraft::isActiveFrequencyWithin25kHzChannel(const CFrequency &comFrequency) const
        {
            return m_com1system.isActiveFrequencyWithin25kHzChannel(comFrequency) ||
                   m_com2system.isActiveFrequencyWithin25kHzChannel(comFrequency);
        }

        bool CSimulatedAircraft::isActiveFrequencyWithinChannelSpacing(const CFrequency &comFrequency) const
        {
            return m_com1system.isActiveFrequencyWithinChannelSpacing(comFrequency) ||
                   m_com2system.isActiveFrequencyWithinChannelSpacing(comFrequency);
        }

        bool CSimulatedAircraft::setTransponderMode(CTransponder::TransponderMode mode)
        {
            return (m_transponder.setTransponderMode(mode));
        }

        QString CSimulatedAircraft::convertToQString(bool i18n) const
        {
            const QString s = m_callsign.toQString(i18n) %
                              u' ' % m_pilot.toQString(i18n) %
                              u' ' % m_situation.toQString(i18n) %
                              u' ' % m_com1system.toQString(i18n) %
                              u' ' % m_com2system.toQString(i18n) %
                              u' ' % m_transponder.toQString(i18n) %
                              u" enabled: " % BlackMisc::boolToYesNo(this->isEnabled()) %
                              u" rendered: " % BlackMisc::boolToYesNo(this->isRendered()) %
                              u' ' % this->getModel().toQString(i18n);
            return s;
        }
    } // namespace
} // namespace
