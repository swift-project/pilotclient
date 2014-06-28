/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_simulator_impl.h"
#include "context_ownaircraft.h"
#include "context_settings.h"
#include "context_application.h"
#include <QPluginLoader>
#include <QLibrary>
#include "context_runtime.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackSim;

namespace BlackCore
{
    CContextSimulator::CContextSimulator(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextSimulator(mode, runtime),
        m_simulator(nullptr), m_updateTimer(nullptr)
    {
        m_updateTimer = new QTimer(this);
        findSimulatorPlugins();
        connect(m_updateTimer, &QTimer::timeout, this, &CContextSimulator::updateOwnAircraft);

        // do not load plugin here, as it depends on settings
        // it has to be guaranteed the settings are alredy loaded
    }

    CContextSimulator::~CContextSimulator()
    {
        disconnectFrom();
        unloadSimulatorPlugin();
    }

    CSimulatorInfoList CContextSimulator::getAvailableSimulatorPlugins() const
    {
        CSimulatorInfoList simulatorPlugins;
        foreach(ISimulatorFactory * factory, m_simulatorFactories)
        {
            simulatorPlugins.push_back(factory->getSimulatorInfo());
        }
        simulatorPlugins.sortBy(&CSimulatorInfo::getShortName);
        return simulatorPlugins;
    }

    bool CContextSimulator::isConnected() const
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator) return false;
        return m_simulator->isConnected();
    }

    bool CContextSimulator::canConnect()
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator) return false;
        return m_simulator->canConnect();
    }

    bool CContextSimulator::connectTo()
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator) return false;
        return m_simulator->connectTo();
    }

    void CContextSimulator::asyncConnectTo()
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator || m_canConnectResult.isRunning()) return; // already checking
        m_simulator->asyncConnectTo();
    }

    bool CContextSimulator::disconnectFrom()
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator) return false;
        return m_simulator->disconnectFrom();
    }

    BlackSim::CSimulatorInfo CContextSimulator::getSimulatorInfo() const
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator) return BlackSim::CSimulatorInfo::UnspecifiedSim();
        return m_simulator->getSimulatorInfo();
    }

    Network::CAircraftModel CContextSimulator::getOwnAircraftModel() const
    {
        // If no ISimulator object is available, return a dummy.
        if (!m_simulator)
            return Network::CAircraftModel();

        return this->m_simulator->getAircraftModel();
    }

    bool CContextSimulator::loadSimulatorPlugin(const CSimulatorInfo &simulatorInfo)
    {
        if (this->m_simulator && this->m_simulator->getSimulatorInfo() == simulatorInfo) { return true; } // already loaded
        if (simulatorInfo.isUnspecified()) { return false; }

        // warning if we do not have any plugins
        if (m_simulatorFactories.isEmpty())
        {
            this->getRuntime()->sendStatusMessage(CStatusMessage::getErrorMessage("No simulator plugins", CStatusMessage::TypeSimulator));
            return false;
        }

        ISimulatorFactory *factory = nullptr;
        QSet<ISimulatorFactory *>::iterator iterator = std::find_if(m_simulatorFactories.begin(), m_simulatorFactories.end(), [ = ](const ISimulatorFactory * factory)
        {
            return factory->getSimulatorInfo() == simulatorInfo;
        });

        // no plugin found
        if (iterator == m_simulatorFactories.end())
        {
            QString m = QString("Plugin not found: '%1'").arg(simulatorInfo.toQString(true));
            this->getRuntime()->sendStatusMessage(CStatusMessage::getErrorMessage(m, CStatusMessage::TypeSimulator));
            qCritical() << m;
            return false;
        }

        factory = *iterator;
        Q_ASSERT(factory);

        ISimulator *newSimulator = factory->create(this);
        Q_ASSERT(newSimulator);

        this->unloadSimulatorPlugin(); // old plugin unloaded
        m_simulator = newSimulator;

        connect(m_simulator, SIGNAL(statusChanged(ISimulator::Status)), this, SLOT(setConnectionStatus(ISimulator::Status)));
        connect(m_simulator, &ISimulator::aircraftModelChanged, this, &IContextSimulator::ownAircraftModelChanged);
        asyncConnectTo(); // try to connect

        QString m = QString("Simulator plugin loaded: '%1'").arg(this->m_simulator->getSimulatorInfo().toQString(true));
        this->getRuntime()->sendStatusMessage(CStatusMessage::getInfoMessage(m, CStatusMessage::TypeSimulator));
        qDebug() << m;
        return true;
    }

    bool CContextSimulator::loadSimulatorPluginFromSettings()
    {
        Q_ASSERT(this->getIContextSettings());
        if (!this->getIContextSettings()) return false;

        CSimulatorInfoList plugin = this->getAvailableSimulatorPlugins();
        if (plugin.size() == 1)
        {
            // load, independent from settings, we have only driver
            return this->loadSimulatorPlugin(plugin.front());
        }
        else if (plugin.size() > 1)
        {
            return this->loadSimulatorPlugin(
                       this->getIContextSettings()->getSimulatorSettings().getSelectedDriver()
                   );
        }
        else
        {
            return false;
        }
    }

    void CContextSimulator::unloadSimulatorPlugin()
    {
        if (m_simulator)
        {
            disconnect(m_simulator); // disconnect as receiver straight away
            m_simulator->disconnectFrom(); // disconnect from simulator
            m_simulator->deleteLater();
        }
        m_simulator = nullptr;
    }

    void CContextSimulator::updateOwnAircraft()
    {
        Q_ASSERT(this->getIContextOwnAircraft());

        // we make sure not to override values we do not have
        CAircraft aircraft = this->getIContextOwnAircraft()->getOwnAircraft();
        CAircraft simulatorAircraft = this->m_simulator->getOwnAircraft();
        aircraft.setSituation(simulatorAircraft.getSituation());
        aircraft.setCockpit(simulatorAircraft.getCom1System(), simulatorAircraft.getCom2System(), simulatorAircraft.getTransponderCode());

        Q_ASSERT(this->getIContextOwnAircraft()); // paranoia against context having been deleted from another thread - redmine issue #270
        if (this->getIContextOwnAircraft())
        {
            // the method will check, if an update is really required
            // these are local (non DBus) calls
            this->getIContextOwnAircraft()->updateOwnAircraft(aircraft, this->getPathAndContextId());
        }
    }

    void CContextSimulator::addAircraftSituation(const CCallsign &callsign, const CAircraftSituation &initialSituation)
    {
        Q_ASSERT(this->m_simulator);
        if (!this->m_simulator) return;
        this->m_simulator->addAircraftSituation(callsign, initialSituation);
    }

    void CContextSimulator::updateCockpitFromContext(const CAircraft &ownAircraft, const QString &originator)
    {
        Q_ASSERT(this->m_simulator);
        if (!this->m_simulator) return;

        // avoid loops
        if (originator.isEmpty() || originator == IContextSimulator::InterfaceName()) return;

        // update
        this->m_simulator->updateOwnSimulatorCockpit(ownAircraft);
    }

    void CContextSimulator::setConnectionStatus(ISimulator::Status status)
    {
        if (status == ISimulator::Connected)
        {
            m_updateTimer->start(100);
            emit connectionChanged(true);
        }
        else
        {
            m_updateTimer->stop();
            emit connectionChanged(false);
        }
    }

    void CContextSimulator::statusMessageReceived(const CStatusMessage &statusMessage)
    {
        if (statusMessage.getSeverity() != CStatusMessage::SeverityError) return;
        this->m_simulator->displayStatusMessage(statusMessage);
    }

    void CContextSimulator::statusMessagesReceived(const CStatusMessageList &statusMessages)
    {
        foreach(CStatusMessage m, statusMessages)
        {
            this->statusMessageReceived(m);
        }
    }

    void CContextSimulator::textMessagesReceived(const Network::CTextMessageList &textMessages)
    {
        if (!this->m_simulator) return;
        foreach(CTextMessage tm, textMessages)
        {
            if (!tm.isPrivateMessage()) continue;
            this->m_simulator->displayStatusMessage(tm.asStatusMessage(true, true));
        }
    }

    void CContextSimulator::settingsChanged(uint type)
    {
        Q_ASSERT(this->getIContextSettings());
        if (!this->getIContextSettings()) return;
        IContextSettings::SettingsType settingsType = static_cast<IContextSettings::SettingsType>(type);
        if (settingsType == IContextSettings::SettingsSimulator)
        {
            CSimulatorInfo driver = this->getIContextSettings()->getSimulatorSettings().getSelectedDriver();
            if (this->loadSimulatorPlugin(driver))
            {
                QString m = QString("Driver loaded: '%1'").arg(driver.toQString(true));
                this->getRuntime()->sendStatusMessage(CStatusMessage::getInfoMessage(m, CStatusMessage::TypeSimulator));
            }
            else
            {
                QString m = QString("Cannot load driver: '%1'").arg(driver.toQString(true));
                this->getRuntime()->sendStatusMessage(CStatusMessage::getErrorMessage(m, CStatusMessage::TypeSimulator));
            }
        }
    }

    void CContextSimulator::findSimulatorPlugins()
    {
        const QString path = qApp->applicationDirPath().append("/plugins/simulator");
        m_pluginsDir = QDir(path);
        if (!m_pluginsDir.exists())
        {
            qWarning() << "No plugin directory" << m_pluginsDir.currentPath();
            return;
        }

        QStringList fileNames = m_pluginsDir.entryList(QDir::Files);
        fileNames.sort(Qt::CaseInsensitive); // give a certain order, rather than random file order
        foreach(QString fileName, fileNames)
        {
            if (!QLibrary::isLibrary(fileName)) { continue; }
            QString pluginPath = m_pluginsDir.absoluteFilePath(fileName);
            QPluginLoader loader(pluginPath);
            QObject *plugin = loader.instance();
            if (plugin)
            {
                ISimulatorFactory *factory = qobject_cast<ISimulatorFactory *>(plugin);
                if (factory)
                {
                    CSimulatorInfo simulatorInfo = factory->getSimulatorInfo();
                    qDebug() << "Found simulator plugin: " << simulatorInfo.toQString();
                    m_simulatorFactories.insert(factory);
                }
            }
            else
            {
                qDebug() << loader.errorString();
                qDebug() << "Also check if required dll/libs of plugin exists";
            }
        }
    }

} // namespace
