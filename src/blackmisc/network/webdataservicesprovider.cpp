/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "webdataservicesprovider.h"

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Weather;

namespace BlackMisc
{
    namespace Network
    {
        CWebDataServicesAware::~CWebDataServicesAware()
        {
            this->m_swiftConnections.disconnectAll();
        }

        CServerList CWebDataServicesAware::getVatsimFsdServers() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CServerList(); }
            return this->m_webDataReaderProvider->getVatsimFsdServers();
        }

        CServerList CWebDataServicesAware::getVatsimVoiceServers() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CServerList(); }
            return this->m_webDataReaderProvider->getVatsimFsdServers();
        }

        CUserList CWebDataServicesAware::getUsersForCallsign(const CCallsign &callsign) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CUserList(); }
            return this->m_webDataReaderProvider->getUsersForCallsign(callsign);
        }

        CAtcStationList CWebDataServicesAware::getAtcStationsForCallsign(const CCallsign &callsign) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CAtcStationList(); }
            return this->m_webDataReaderProvider->getAtcStationsForCallsign(callsign);
        }

        CVoiceCapabilities CWebDataServicesAware::getVoiceCapabilityForCallsign(const CCallsign &callsign) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CVoiceCapabilities(); }
            return this->m_webDataReaderProvider->getVoiceCapabilityForCallsign(callsign);
        }

        CDistributorList CWebDataServicesAware::getDistributors() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CDistributorList(); }
            return this->m_webDataReaderProvider->getDistributors();
        }

        CDistributor CWebDataServicesAware::smartDistributorSelector(const CDistributor &distributor) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CDistributor(); }
            return this->m_webDataReaderProvider->smartDistributorSelector(distributor);
        }

        int CWebDataServicesAware::getDistributorsCount() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return 0; }
            return this->m_webDataReaderProvider->getDistributorsCount();
        }

        CLiveryList CWebDataServicesAware::getLiveries() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CLiveryList(); }
            return this->m_webDataReaderProvider->getLiveries();
        }

        int CWebDataServicesAware::getLiveriesCount() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return 0; }
            return this->m_webDataReaderProvider->getLiveriesCount();
        }

        CLivery CWebDataServicesAware::getLiveryForCombinedCode(const QString &combinedCode) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CLivery(); }
            return this->m_webDataReaderProvider->getLiveryForCombinedCode(combinedCode);
        }

        CLivery CWebDataServicesAware::getStdLiveryForAirlineCode(const CAirlineIcaoCode &icao) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CLivery(); }
            return this->m_webDataReaderProvider->getStdLiveryForAirlineCode(icao);
        }

        CLivery CWebDataServicesAware::smartLiverySelector(const CLivery &livery) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CLivery(); }
            return this->m_webDataReaderProvider->smartLiverySelector(livery);
        }

        CAircraftModelList CWebDataServicesAware::getModels() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CAircraftModelList(); }
            return this->m_webDataReaderProvider->getModels();
        }

        int CWebDataServicesAware::getModelsCount() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return 0; }
            return this->m_webDataReaderProvider->getModelsCount();
        }

        CAircraftModel CWebDataServicesAware::getModelForModelString(const QString &modelString) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CAircraftModel(); }
            return this->m_webDataReaderProvider->getModelForModelString(modelString);
        }

        CAircraftModelList CWebDataServicesAware::getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CAircraftModelList(); }
            return this->m_webDataReaderProvider->getModelsForAircraftDesignatorAndLiveryCombinedCode(aircraftDesignator, combinedCode);
        }

        CAircraftIcaoCodeList CWebDataServicesAware::getAircraftIcaoCodes() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CAircraftIcaoCodeList(); }
            return this->m_webDataReaderProvider->getAircraftIcaoCodes();
        }

        int CWebDataServicesAware::getAircraftIcaoCodesCount() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return 0; }
            return this->m_webDataReaderProvider->getAircraftIcaoCodesCount();
        }

        CAircraftIcaoCode CWebDataServicesAware::getAircraftIcaoCodeForDesignator(const QString &designator) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CAircraftIcaoCode(); }
            return this->m_webDataReaderProvider->getAircraftIcaoCodeForDesignator(designator);
        }

        CAircraftIcaoCode CWebDataServicesAware::getAircraftIcaoCodeForDbKey(int id) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "missing provider");
            if (!hasProvider()) { return CAircraftIcaoCode(); }
            return this->m_webDataReaderProvider->getAircraftIcaoCodeForDbKey(id);
        }

        CAircraftIcaoCode CWebDataServicesAware::smartAircraftIcaoSelector(const CAircraftIcaoCode &icao) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CAircraftIcaoCode(); }
            return this->m_webDataReaderProvider->smartAircraftIcaoSelector(icao);
        }

        CAirlineIcaoCodeList CWebDataServicesAware::getAirlineIcaoCodes() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            return this->m_webDataReaderProvider->getAirlineIcaoCodes();
        }

        int CWebDataServicesAware::getAirlineIcaoCodesCount() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return 0; }
            return this->m_webDataReaderProvider->getAirlineIcaoCodesCount();
        }

        CAirlineIcaoCode CWebDataServicesAware::getAirlineIcaoCodeForDesignator(const QString &designator) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CAirlineIcaoCode(); }
            return this->m_webDataReaderProvider->getAirlineIcaoCodeForDesignator(designator);
        }

        CAirlineIcaoCode CWebDataServicesAware::getAirlineIcaoCodeForDbKey(int id) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CAirlineIcaoCode(); }
            return this->m_webDataReaderProvider->getAirlineIcaoCodeForDbKey(id);
        }

        CCountryList CWebDataServicesAware::getCountries() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CCountryList(); }
            return this->m_webDataReaderProvider->getCountries();
        }

        int CWebDataServicesAware::getCountriesCount() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return 0; }
            return this->m_webDataReaderProvider->getCountriesCount();
        }

        CCountry CWebDataServicesAware::getCountryForName(const QString &name) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CCountry(); }
            return this->m_webDataReaderProvider->getCountryForName(name);
        }

        CMetarSet CWebDataServicesAware::getMetars() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CMetarSet(); }
            return this->m_webDataReaderProvider->getMetars();
        }

        Weather::CMetar CWebDataServicesAware::getMetarForAirport(const CAirportIcaoCode &icao) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CMetar(); }
            return this->m_webDataReaderProvider->getMetarForAirport(icao);
        }

        int CWebDataServicesAware::getMetarCount() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return 0; }
            return this->m_webDataReaderProvider->getMetarsCount();
        }

        CCountry CWebDataServicesAware::getCountryForIsoCode(const QString &iso) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CCountry(); }
            return this->m_webDataReaderProvider->getCountryForIsoCode(iso);
        }

        void CWebDataServicesAware::updateWithVatsimDataFileData(CSimulatedAircraft &aircraftToBeUdpated) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return; }
            return this->m_webDataReaderProvider->updateWithVatsimDataFileData(aircraftToBeUdpated);
        }

        CStatusMessageList CWebDataServicesAware::asyncWriteModel(const CAircraftModel &model) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CStatusMessageList(); }
            return this->m_webDataReaderProvider->asyncWriteModel(model);
        }

        void CWebDataServicesAware::setProvider(IWebDataServicesProvider *webDataReaderProvider)
        {
            Q_ASSERT_X(webDataReaderProvider, Q_FUNC_INFO, "missing provider");
            this->m_swiftConnections.disconnectAll();
            m_webDataReaderProvider = webDataReaderProvider;
        }

        bool CWebDataServicesAware::hasProvider() const
        {
            return m_webDataReaderProvider ? true : false;
        }

        void CWebDataServicesAware::gracefulShutdown()
        {
            this->m_swiftConnections.disconnectAll();
            this->m_webDataReaderProvider = nullptr;
        }

        void CWebDataServicesAware::connectDataReadSignal(QObject *receiver, std::function<void (CEntityFlags::Entity, CEntityFlags::ReadState, int)> dataRead)
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return; }
            if (receiver)
            {
                this->m_swiftConnections.append(this->m_webDataReaderProvider->connectDataReadSignal(receiver, dataRead));
            }
        }

        CEntityFlags::Entity CWebDataServicesAware::triggerRead(CEntityFlags::Entity whatToRead)
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return CEntityFlags::NoEntity; }
            return this->m_webDataReaderProvider->triggerRead(whatToRead);
        }

        bool CWebDataServicesAware::canConnectSwiftDb() const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return false; }
            return this->m_webDataReaderProvider->canConnectSwiftDb();
        }

        bool CWebDataServicesAware::writeDbDataToDisk(const QString &dir) const
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return false; }
            return this->m_webDataReaderProvider->writeDbDataToDisk(dir);
        }

        bool CWebDataServicesAware::readDbDataFromDisk(const QString &dir, bool inBackround)
        {
            Q_ASSERT_X(this->m_webDataReaderProvider, Q_FUNC_INFO, "Missing provider");
            if (!hasProvider()) { return false; }
            return this->m_webDataReaderProvider->readDbDataFromDisk(dir, inBackround);
        }

    } // namespace
} // namespace
