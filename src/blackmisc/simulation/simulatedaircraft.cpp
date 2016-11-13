/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/metaclassprivate.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/verify.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/stringutils.h"

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
            init();
        }

        CSimulatedAircraft::CSimulatedAircraft(const CAircraftModel &model) : m_model(model), m_networkModel(model)
        {
            this->setCallsign(model.getCallsign());
            init();
        }

        CSimulatedAircraft::CSimulatedAircraft(const CCallsign &callsign, const CUser &user, const CAircraftSituation &situation) :
            m_callsign(callsign), m_pilot(user), m_situation(situation)
        {
            init();
        }

        void CSimulatedAircraft::init()
        {
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
            this->m_transponder.setTransponderCode(transponderCode);
            this->m_transponder.setTransponderMode(transponderMode);
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
            if (this->m_callsign.asString().isEmpty()) { return false; }
            if (!this->m_pilot.isValid()) { return false; }
            return true;
        }

        void CSimulatedAircraft::setSituation(const CAircraftSituation &situation)
        {
            m_situation = situation;
            m_situation.setCallsign(this->getCallsign());
        }

        const CAircraftIcaoCode &CSimulatedAircraft::getAircraftIcaoCode() const
        {
            return m_model.getAircraftIcaoCode();
        }

        void CSimulatedAircraft::setPilot(const Network::CUser &user)
        {
            this->m_pilot = user;
            this->m_pilot.setCallsign(this->m_callsign);
        }

        bool CSimulatedAircraft::isEnabled() const
        {
            if (!this->hasModelString()) { return false; }
            return m_enabled;
        }

        const QString &CSimulatedAircraft::getAircraftIcaoCodeDesignator() const
        {
            return getAircraftIcaoCode().getDesignator();
        }

        const QString &CSimulatedAircraft::getAircraftIcaoCombinedType() const
        {
            return getAircraftIcaoCode().getCombinedType();
        }

        bool CSimulatedAircraft::setIcaoCodes(const CAircraftIcaoCode &aircraftIcaoCode, const CAirlineIcaoCode &airlineIcaoCode)
        {
            //! \note to be compatible with old version I still allow to set airline here but I should actually set a livery

            if (this->getLivery().getAirlineIcaoCode() != airlineIcaoCode)
            {
                // create a dummy livery for given ICAO code
                CLivery newLivery(CLivery::getStandardCode(airlineIcaoCode), airlineIcaoCode, "Standard auto generated");
                this->m_model.setLivery(newLivery);
            }
            return this->m_model.setAircraftIcaoCode(aircraftIcaoCode);
        }

        const CAirlineIcaoCode &CSimulatedAircraft::getAirlineIcaoCode() const
        {
            return this->m_model.getAirlineIcaoCode();
        }

        const QString &CSimulatedAircraft::getAirlineIcaoCodeDesignator() const
        {
            return getAirlineIcaoCode().getDesignator();
        }

        void CSimulatedAircraft::setAircraftIcaoDesignator(const QString &designator)
        {
            this->m_model.setAircraftIcaoDesignator(designator);
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
            Q_ASSERT(false);
            return CComSystem(); // avoid warning
        }

        void CSimulatedAircraft::setComSystem(const CComSystem &com, CComSystem::ComUnit unit)
        {
            switch (unit)
            {
            case CComSystem::Com1: this->setCom1System(com); break;
            case CComSystem::Com2: this->setCom2System(com); break;
            }
        }

        bool CSimulatedAircraft::setCom1ActiveFrequency(const CFrequency &frequency)
        {
            if (!CComSystem::isValidComFrequency(frequency)) { return false; }
            this->m_com1system.setFrequencyActive(frequency);
            return true;
        }

        bool CSimulatedAircraft::setCom2ActiveFrequency(const CFrequency &frequency)
        {
            if (!CComSystem::isValidComFrequency(frequency)) { return false; }
            this->m_com2system.setFrequencyActive(frequency);
            return true;
        }

        bool CSimulatedAircraft::setComActiveFrequency(const CFrequency &frequency, CComSystem::ComUnit unit)
        {
            if (!CComSystem::isValidComFrequency(frequency)) { return false; }
            switch (unit)
            {
            case CComSystem::Com1: return this->setCom1ActiveFrequency(frequency);
            case CComSystem::Com2: return this->setCom2ActiveFrequency(frequency);
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
            CTransponder xpdr(7000, CTransponder::StateStandby);
            this->setTransponder(xpdr);
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
            return getAircraftIcaoCode().isVtol();
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

        CVariant CSimulatedAircraft::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModel:
                return this->getModel().propertyByIndex(index.copyFrontRemoved());
            case IndexNetworkModel:
                return this->getNetworkModel().propertyByIndex(index.copyFrontRemoved());
            case IndexNetworkModelAircraftIcaoDifference:
                return this->getNetworkModelAircraftIcaoDifference();
            case IndexNetworkModelAirlineIcaoDifference:
                return this->getNetworkModelAirlineIcaoDifference();
            case IndexNetworkModelLiveryDifference:
                return this->getNetworkModelLiveryDifference();
            case IndexEnabled:
                return CVariant::fromValue(this->isEnabled());
            case IndexRendered:
                return CVariant::fromValue(this->isRendered());
            case IndexPartsSynchronized:
                return CVariant::fromValue(this->isPartsSynchronized());
            case IndexFastPositionUpdates:
                return CVariant::fromValue(this->fastPositionUpdates());
            case IndexCallsign:
                return this->m_callsign.propertyByIndex(index.copyFrontRemoved());
            case IndexPilot:
                return this->m_pilot.propertyByIndex(index.copyFrontRemoved());
            case IndexRelativeDistance:
                return this->m_relativeDistance.propertyByIndex(index.copyFrontRemoved());
            case IndexCom1System:
                return this->m_com1system.propertyByIndex(index.copyFrontRemoved());
            case IndexCom2System:
                return this->m_com2system.propertyByIndex(index.copyFrontRemoved());
            case IndexTransponder:
                return this->m_transponder.propertyByIndex(index.copyFrontRemoved());
            case IndexSituation:
                return this->m_situation.propertyByIndex(index.copyFrontRemoved());
            case IndexAircraftIcaoCode:
                return this->getAircraftIcaoCode().propertyByIndex(index.copyFrontRemoved());
            case IndexLivery:
                return this->getLivery().propertyByIndex(index.copyFrontRemoved());
            case IndexParts:
                return this->m_parts.propertyByIndex(index.copyFrontRemoved());
            case IndexIsVtol:
                return CVariant::fromValue(this->isVtol());
            case IndexCombinedIcaoLiveryString:
                return CVariant::fromValue(this->getCombinedIcaoLiveryString(false));
            case IndexCombinedIcaoLiveryStringNetworkModel:
                return CVariant::fromValue(this->getCombinedIcaoLiveryString(true));
            default:
                return (ICoordinateWithRelativePosition::canHandleIndex(index)) ?
                       ICoordinateWithRelativePosition::propertyByIndex(index) :
                       CValueObject::propertyByIndex(index);
            }
        }

        void CSimulatedAircraft::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CSimulatedAircraft>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsign:
                this->m_callsign.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexPilot:
                this->m_pilot.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexRelativeDistance:
                this->m_relativeDistance.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexCom1System:
                this->m_com1system.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexCom2System:
                this->m_com2system.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexTransponder:
                this->m_transponder.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexSituation:
                this->m_situation.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexParts:
                this->m_parts.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexModel:
                this->m_model.setPropertyByIndex(index.copyFrontRemoved(), variant);
                this->setModel(this->m_model); // sync some values
                break;
            case IndexNetworkModel:
                this->m_networkModel.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexEnabled:
                this->m_enabled = variant.toBool();
                break;
            case IndexRendered:
                this->m_rendered = variant.toBool();
                break;
            case IndexPartsSynchronized:
                this->m_partsSynchronized = variant.toBool();
                break;
            case IndexFastPositionUpdates:
                this->m_fastPositionUpdates = variant.toBool();
                break;
            case IndexLivery:
                Q_ASSERT_X(false, Q_FUNC_INFO, "Unsupported");
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

        int CSimulatedAircraft::comparePropertyByIndex(const CPropertyIndex &index, const CSimulatedAircraft &compareValue) const
        {
            if (index.isMyself()) { return this->m_callsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign()); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsign:
                return this->m_callsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
            case IndexPilot:
                return this->m_pilot.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getPilot());
            case IndexSituation:
                return this->m_situation.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getSituation());
            case IndexRelativeDistance:
                return this->m_relativeDistance.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getRelativeDistance());
            case IndexCom1System:
                return m_com1system.getFrequencyActive().comparePropertyByIndex(CPropertyIndex(), compareValue.getCom1System().getFrequencyActive());
            case IndexCom2System:
                return m_com2system.getFrequencyActive().comparePropertyByIndex(CPropertyIndex(), compareValue.getCom2System().getFrequencyActive());
            case IndexTransponder:
                return Compare::compare(m_transponder.getTransponderCode(), compareValue.getTransponder().getTransponderCode());
            case IndexLivery:
                return this->getLivery().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getLivery());
            case IndexParts:
                return this->m_parts.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getParts());
            case IndexModel:
                return m_model.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getModel());
            case IndexNetworkModel:
                return m_networkModel.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getModel());
            case IndexNetworkModelAircraftIcaoDifference:
                return this->getNetworkModelAircraftIcaoDifference().compare(compareValue.getNetworkModelAircraftIcaoDifference());
            case IndexNetworkModelAirlineIcaoDifference:
                return this->getNetworkModelAirlineIcaoDifference().compare(compareValue.getNetworkModelAirlineIcaoDifference());
            case IndexNetworkModelLiveryDifference:
                return this->getNetworkModelLiveryDifference().compare(compareValue.getNetworkModelLiveryDifference());
            case IndexRendered:
                return Compare::compare(this->m_rendered, compareValue.isRendered());
            case IndexPartsSynchronized:
                return Compare::compare(this->m_partsSynchronized, compareValue.isPartsSynchronized());
            case IndexFastPositionUpdates:
                return Compare::compare(this->m_fastPositionUpdates, compareValue.fastPositionUpdates());
            case IndexCombinedIcaoLiveryString:
                return this->getCombinedIcaoLiveryString(false).compare(compareValue.getCombinedIcaoLiveryString(false));
            case IndexCombinedIcaoLiveryStringNetworkModel:
                return this->getCombinedIcaoLiveryString(true).compare(compareValue.getCombinedIcaoLiveryString(true));
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
            return this->hasNetworkModel() ? this->m_networkModel : this->m_model;
        }

        bool CSimulatedAircraft::hasNetworkModel() const
        {
            return this->m_networkModel.hasModelString() || !this->m_networkModel.getCallsign().isEmpty();
        }

        QString CSimulatedAircraft::getNetworkModelAircraftIcaoDifference() const
        {
            const CAircraftIcaoCode icao(this->getModel().getAircraftIcaoCode());
            const CAircraftIcaoCode icaoNw(this->getNetworkModel().getAircraftIcaoCode());
            if (icao == icaoNw || icao.getDesignator() == icaoNw.getDesignator()) { return "[=] " + icao.getDesignator(); }
            static const QString diff("%1 -> %2");
            return diff.arg(icaoNw.getDesignator(), icao.getDesignator());
        }

        QString CSimulatedAircraft::getNetworkModelAirlineIcaoDifference() const
        {
            const CAirlineIcaoCode icao(this->getModel().getAirlineIcaoCode());
            const CAirlineIcaoCode icaoNw(this->getNetworkModel().getAirlineIcaoCode());
            if (icao == icaoNw || icao.getDesignator() == icaoNw.getDesignator()) { return "[=] " + icao.getDesignator(); }
            static const QString diff("%1 -> %2");
            return diff.arg(icaoNw.getDesignator(), icao.getDesignator());
        }

        QString CSimulatedAircraft::getNetworkModelLiveryDifference() const
        {
            const CLivery livery(this->getModel().getLivery());
            const CLivery liveryNw(this->getNetworkModel().getLivery());
            if (livery == liveryNw) { return "[=] " + livery.getCombinedCodePlusInfo(); }
            static const QString diff("%1 -> %2");
            return diff.arg(liveryNw.getCombinedCodePlusInfo(), livery.getCombinedCodePlusInfo());
        }

        void CSimulatedAircraft::setModel(const CAircraftModel &model)
        {
            // sync the callsigns
            this->m_model = model;
            this->setCallsign(this->hasValidCallsign() ? this->getCallsign() : model.getCallsign());
            this->setIcaoCodes(model.getAircraftIcaoCode(), model.getAirlineIcaoCode());
        }

        void CSimulatedAircraft::setNetworkModel(const CAircraftModel &model)
        {
            this->m_networkModel = model;
        }

        void CSimulatedAircraft::setModelString(const QString &modelString)
        {
            this->m_model.setModelString(modelString);
        }

        void CSimulatedAircraft::setCallsign(const CCallsign &callsign)
        {
            this->m_callsign = callsign;
            this->m_model.setCallsign(callsign);
            this->m_networkModel.setCallsign(callsign);
            this->m_pilot.setCallsign(callsign);
        }

        bool CSimulatedAircraft::isActiveFrequencyWithin8_33kHzChannel(const CFrequency &comFrequency) const
        {
            return this->m_com1system.isActiveFrequencyWithin8_33kHzChannel(comFrequency) ||
                   this->m_com2system.isActiveFrequencyWithin8_33kHzChannel(comFrequency);
        }

        bool CSimulatedAircraft::isActiveFrequencyWithin25kHzChannel(const CFrequency &comFrequency) const
        {
            return this->m_com1system.isActiveFrequencyWithin25kHzChannel(comFrequency) ||
                   this->m_com2system.isActiveFrequencyWithin25kHzChannel(comFrequency);
        }

        QString CSimulatedAircraft::convertToQString(bool i18n) const
        {
            QString s(this->m_callsign.toQString(i18n));
            s += " ";
            s += this->m_pilot.toQString(i18n);
            s += " ";
            s += this->m_situation.toQString(i18n);
            s += " ";
            s += this->m_com1system.toQString(i18n);
            s += " ";
            s += this->m_com2system.toQString(i18n);
            s += " ";
            s += this->m_transponder.toQString(i18n);
            s += " enabled: ";
            s +=  BlackMisc::boolToYesNo(this->isEnabled());
            s += " ";
            s += " rendered: ";
            s +=  BlackMisc::boolToYesNo(this->isRendered());
            s += " ";
            s += this->m_model.toQString(i18n);
            return s;
        }
    } // namespace
} // namespace
