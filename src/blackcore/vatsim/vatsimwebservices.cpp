// SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "vatsimwebservices.h"
#include "blackcore/application.h"
#include "blackcore/vatsim/vatsimdatafilereader.h"
#include "blackcore/vatsim/vatsimmetarreader.h"
#include "blackcore/vatsim/vatsimstatusfilereader.h"
#include "blackcore/vatsim/vatsimserverfilereader.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;

namespace BlackCore::Vatsim
{

    CVatsimWebServices::CVatsimWebServices(BlackMisc::Restricted<CApplication>, QObject *parent) : QObject(parent)
    {
        if (!sApp) { return; } // shutting down

        Q_ASSERT_X(QSslSocket::supportsSsl(), Q_FUNC_INFO, "Missing SSL support");
        Q_ASSERT_X(sApp->isSetupAvailable(), Q_FUNC_INFO, "Setup not synchronized");
        this->setObjectName("CVatsimWebServices");

        // SSL INFOs
        CLogMessage(this).info(u"SSL supported: %1 Version: %2 (build version) %3 (library version)") << boolToYesNo(QSslSocket::supportsSsl()) << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();

        this->runVatsimStatusReader();
    }

    const QStringList &CVatsimWebServices::getLogCategories()
    {
        static const QStringList cats { "swift.vatsimwebservices", CLogCategories::webservice() };
        return cats;
    }

    CVatsimWebServices::~CVatsimWebServices()
    {
        this->gracefulShutdown();
    }

    CServerList CVatsimWebServices::getVatsimFsdServers() const
    {
        if (m_vatsimServerFileReader) { return m_vatsimServerFileReader->getFsdServers(); }
        return {};
    }

    CUrl CVatsimWebServices::getVatsimMetarUrl() const
    {
        if (m_vatsimStatusReader) { return m_vatsimStatusReader->getMetarFileUrl(); }
        return {};
    }

    CUrl CVatsimWebServices::getVatsimDataFileUrl() const
    {
        if (m_vatsimStatusReader) { return m_vatsimStatusReader->getDataFileUrl(); }
        return {};
    }

    CUserList CVatsimWebServices::getUsersForCallsign(const CCallsign &callsign) const
    {
        if (m_vatsimDataFileReader) { return m_vatsimDataFileReader->getUsersForCallsign(callsign); }
        return {};
    }

    CAtcStationList CVatsimWebServices::getAtcStationsForCallsign(const CCallsign &callsign) const
    {
        if (m_vatsimDataFileReader) { return m_vatsimDataFileReader->getAtcStationsForCallsign(callsign); }
        return {};
    }

    CVoiceCapabilities CVatsimWebServices::getVoiceCapabilityForCallsign(const CCallsign &callsign) const
    {
        if (m_vatsimDataFileReader) { return m_vatsimDataFileReader->getVoiceCapabilityForCallsign(callsign); }
        return {};
    }

    void CVatsimWebServices::updateWithVatsimDataFileData(CSimulatedAircraft &aircraftToBeUpdated) const
    {
        if (m_vatsimDataFileReader) { m_vatsimDataFileReader->updateWithVatsimDataFileData(aircraftToBeUpdated); }
    }

    CMetar CVatsimWebServices::getMetarForAirport(const CAirportIcaoCode &icao) const
    {
        if (m_vatsimMetarReader) { return m_vatsimMetarReader->getMetarForAirport(icao); }
        return {};
    }

    void CVatsimWebServices::gracefulShutdown()
    {
        if (m_shuttingDown) { return; }
        m_shuttingDown = true;
        this->disconnect(); // all signals
        if (m_vatsimMetarReader)
        {
            m_vatsimMetarReader->quitAndWait();
            m_vatsimMetarReader = nullptr;
        }
        if (m_vatsimDataFileReader)
        {
            m_vatsimDataFileReader->quitAndWait();
            m_vatsimDataFileReader = nullptr;
        }
        if (m_vatsimStatusReader)
        {
            m_vatsimStatusReader->quitAndWait();
            m_vatsimStatusReader = nullptr;
        }
        if (m_vatsimServerFileReader)
        {
            m_vatsimServerFileReader->quitAndWait();
            m_vatsimServerFileReader = nullptr;
        }
    }

    void CVatsimWebServices::receivedMetars(const CMetarList &metars)
    {
        CLogMessage(this).info(u"Read %1 METARs") << metars.size();
    }

    void CVatsimWebServices::vatsimDataFileRead(int kB)
    {
        CLogMessage(this).info(u"Read VATSIM data file, %1 kB") << kB;
    }

    void CVatsimWebServices::vatsimStatusFileRead(double kB)
    {
        CLogMessage(this).info(u"Read VATSIM status file, %1 kB") << kB;

        startVatsimServerFileReader();
        startVatsimDataFileReader();
        startVatsimMetarReader();
    }

    void CVatsimWebServices::vatsimServerFileRead(double kB, [[maybe_unused]] const QUrl &url)
    {
        CLogMessage(this).info(u"Read VATSIM server file, %1 kB") << kB;
    }

    void CVatsimWebServices::runVatsimStatusReader()
    {
        Q_ASSERT_X(CThreadUtils::thisIsMainThread(), Q_FUNC_INFO, "shall run in main application thread");

        m_vatsimStatusReader = new CVatsimStatusFileReader(this);
        connect(m_vatsimStatusReader, &CVatsimStatusFileReader::dataRead, this, &CVatsimWebServices::vatsimStatusFileRead, Qt::QueuedConnection);
        m_vatsimStatusReader->start(QThread::LowPriority);

        CLogMessage(this).info(u"Trigger read of VATSIM status file");
        const QPointer<CVatsimWebServices> myself(this);
        QTimer::singleShot(0, this, [=]() {
            if (!myself || m_shuttingDown) { return; }
            if (!sApp || sApp->isShuttingDown()) { return; }
            m_vatsimStatusReader->readInBackgroundThread();
        });
    }

    void CVatsimWebServices::startVatsimServerFileReader()
    {
        if (m_vatsimServerFileReader) return;

        m_vatsimServerFileReader = new CVatsimServerFileReader(this);
        connect(m_vatsimServerFileReader, &CVatsimServerFileReader::dataRead, this, &CVatsimWebServices::vatsimServerFileRead, Qt::QueuedConnection);
        m_vatsimServerFileReader->start(QThread::LowPriority);

        CLogMessage(this).info(u"Trigger read of VATSIM server file");
        const QPointer<CVatsimWebServices> myself(this);
        QTimer::singleShot(0, this, [=]() {
            if (!myself || m_shuttingDown) { return; }
            if (!sApp || sApp->isShuttingDown()) { return; }
            m_vatsimServerFileReader->readInBackgroundThread();
        });
    }

    void CVatsimWebServices::startVatsimDataFileReader()
    {
        if (m_vatsimDataFileReader) return;

        m_vatsimDataFileReader = new CVatsimDataFileReader(this);
        const bool c = connect(m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, this, &CVatsimWebServices::vatsimDataFileRead, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM data reader signals");
        Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed VATSIM data file");
        m_vatsimDataFileReader->start(QThread::LowPriority);
        CLogMessage(this).info(u"Start periodically reading VATSIM data file");
        m_vatsimDataFileReader->startReader();
    }

    void CVatsimWebServices::startVatsimMetarReader()
    {
        m_vatsimMetarReader = new CVatsimMetarReader(this);
        const bool c = connect(m_vatsimMetarReader, &CVatsimMetarReader::metarsRead, this, &CVatsimWebServices::receivedMetars, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM METAR reader signals");
        Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed VATSIM METAR");
        m_vatsimMetarReader->start(QThread::LowPriority);
        CLogMessage(this).info(u"Start periodically reading VATSIM METAR file");

        m_vatsimMetarReader->startReader();
    }
}
