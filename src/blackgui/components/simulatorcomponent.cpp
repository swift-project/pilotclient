/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ui_simulatorcomponent.h"
#include "blackcore/simulator.h"
#include "blackgui/components/simulatorcomponent.h"
#include "blackgui/views/namevariantpairview.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextsimulator.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/stringutils.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        const CLogCategoryList &CSimulatorComponent::getLogCategories()
        {
            static const CLogCategoryList cats { CLogCategory::guiComponent(), CLogCategory::matching() };
            return cats;
        }

        CSimulatorComponent::CSimulatorComponent(QWidget *parent) :
            QTabWidget(parent),
            CEnableForDockWidgetInfoArea(),
            ui(new Ui::CSimulatorComponent)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");

            ui->setupUi(this);
            this->setCurrentIndex(0);
            ui->comp_StatusMessages->showFilterDialog();

            // live data and internals
            ui->tvp_LiveData->setIconMode(true);
            ui->tvp_LiveData->setAutoResizeFrequency(10); // only resize every n-th time
            ui->tvp_Internals->setIconMode(false);
            this->addOrUpdateLiveDataByName("info", "no data yet", CIcons::StandardIconWarning16);

            // connects
            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CSimulatorComponent::onSimulatorStatusChanged);
            connect(&m_updateTimer, &QTimer::timeout, this, &CSimulatorComponent::update);
            connect(ui->pb_RefreshInternals, &QPushButton::pressed, this, &CSimulatorComponent::refreshInternals);
            if (sGui->supportsContexts() && sGui->getIContextSimulator())
            {
                connect(sGui->getIContextSimulator(), &IContextSimulator::addingRemoteModelFailed, this, &CSimulatorComponent::onAddingRemoteModelFailed, Qt::QueuedConnection);
                connect(sGui->getIContextSimulator(), &IContextSimulator::driverMessages, this, &CSimulatorComponent::onSimulatorMessages, Qt::QueuedConnection);
            }

            // init status
            this->onSimulatorStatusChanged(sGui->getIContextSimulator()->getSimulatorStatus());
        }

        CSimulatorComponent::~CSimulatorComponent()
        { }

        void CSimulatorComponent::addOrUpdateLiveDataByName(const QString &name, const QString &value, const CIcon &icon)
        {
            const bool resize = this->currentWidget() == ui->tb_LiveData; // simulator live data selected?
            ui->tvp_LiveData->addOrUpdateByName(name, value, icon, resize, false);
        }

        void CSimulatorComponent::addOrUpdateLiveDataByName(const QString &name, const QString &value, CIcons::IconIndex iconIndex)
        {
            this->addOrUpdateLiveDataByName(name, value, CIcon::iconByIndex(iconIndex));
        }

        void CSimulatorComponent::removeLiveDataByName(const QString &name)
        {
            ui->tvp_LiveData->removeByName(name);
        }

        int CSimulatorComponent::rowCount() const
        {
            return ui->tvp_LiveData->rowCount();
        }

        void CSimulatorComponent::clear(bool addInternalsAfterwards)
        {
            ui->tvp_LiveData->clear();
            if (addInternalsAfterwards)
            {
                this->refreshInternals();
            }
        }

        void CSimulatorComponent::update()
        {
            if (!this->isVisibleWidget()) return; // no updates on invisible widgets
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) return;

            ISimulator::SimulatorStatus simulatorStatus = static_cast<ISimulator::SimulatorStatus>(sGui->getIContextSimulator()->getSimulatorStatus());
            if (simulatorStatus == ISimulator::Unspecified || simulatorStatus == ISimulator::Disconnected)
            {
                static const QString s("No simulator available");
                this->addOrUpdateLiveDataByName("info", s, CIcons::StandardIconWarning16);
                return;
            }

            if (!simulatorStatus.testFlag(ISimulator::Simulating))
            {
                static const QString s("Simulator (%1) not yet running");
                this->addOrUpdateLiveDataByName("info", s.arg(sGui->getIContextSimulator()->getSimulatorPluginInfo().getSimulator()), CIcons::StandardIconWarning16);
                return;
            }

            // clear old warnings / information
            if (this->rowCount() < 5)
            {
                this->clear(true);
            }

            const CSimulatedAircraft ownAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
            const CAircraftSituation s = ownAircraft.getSituation();
            const CComSystem c1 = ownAircraft.getCom1System();
            const CComSystem c2 = ownAircraft.getCom2System();
            static const CIcon iconAlt(s.getAltitude().toIcon()); // minor performance improvement
            static const CIcon iconLatLng(s.latitude().toIcon());
            static const CIcon iconRadio(CIcon::iconByIndex(CIcons::StandardIconRadio16));
            static const CIcon iconAttitude(CIcon::iconByIndex(CIcons::AviationAttitudeIndicator));

            if (m_simulator.isAnySimulator())
            {
                this->addOrUpdateLiveDataByName("simulator", m_simulator.toQString(true), m_simulator.toIcon());
            }
            this->addOrUpdateLiveDataByName("latitude", s.latitude().toQString(), iconLatLng);
            this->addOrUpdateLiveDataByName("longitude", s.longitude().toQString(), iconLatLng);
            this->addOrUpdateLiveDataByName("altitude", s.getAltitude().valueRoundedWithUnit(CLengthUnit::ft(), 2), iconAlt);
            if (s.hasGroundElevation())
            {
                this->addOrUpdateLiveDataByName("elevation", s.getGroundElevation().valueRoundedWithUnit(CLengthUnit::ft(), 2), iconAlt);
            }
            this->addOrUpdateLiveDataByName("pitch", s.getPitch().toQString(), iconAttitude);
            this->addOrUpdateLiveDataByName("bank", s.getBank().toQString(), iconAttitude);
            this->addOrUpdateLiveDataByName("heading", s.getHeading().toQString(), s.getHeading().toIcon());
            this->addOrUpdateLiveDataByName("ground speed", s.getGroundSpeed().toQString(), s.getGroundSpeed().toIcon());

            this->addOrUpdateLiveDataByName("COM1 active", c1.getFrequencyActive().toQString(), iconRadio);
            this->addOrUpdateLiveDataByName("COM2 active", c2.getFrequencyActive().toQString(), iconRadio);
            this->addOrUpdateLiveDataByName("COM1 standby", c1.getFrequencyStandby().toQString(), iconRadio);
            this->addOrUpdateLiveDataByName("COM2 standby", c2.getFrequencyStandby().toQString(), iconRadio);
            this->addOrUpdateLiveDataByName("Transponder", ownAircraft.getTransponderCodeFormatted(), iconRadio);
        }

        void CSimulatorComponent::onSimulatorStatusChanged(int status)
        {
            ISimulator::SimulatorStatus simStatus = static_cast<ISimulator::SimulatorStatus>(status);
            this->clear(); // clean up, will be refreshed
            if (simStatus.testFlag(ISimulator::Connected))
            {
                const int intervalMs = getUpdateIntervalMs();
                m_updateTimer.start(intervalMs);
                this->clear(true);
            }
            else
            {
                m_updateTimer.stop();
                this->update();
            }
        }

        void CSimulatorComponent::onAddingRemoteModelFailed(const CSimulatedAircraft &aircraft, const CStatusMessage &message)
        {
            ui->comp_StatusMessages->appendStatusMessageToList(CStatusMessage(this).warning("Adding model failed: '%1'") << aircraft.toQString(true));
            ui->comp_StatusMessages->appendStatusMessageToList(message);
        }

        void CSimulatorComponent::onSimulatorMessages(const BlackMisc::CStatusMessageList &messages)
        {
            if (messages.isEmpty()) { return; }
            ui->comp_StatusMessages->appendStatusMessagesToList(messages);
        }

        void CSimulatorComponent::refreshInternals()
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }
            const CSimulatorInternals internals = sGui->getIContextSimulator()->getSimulatorInternals();
            const CSimulatorInfo simulatorInfo = sGui->getIContextSimulator()->getSimulatorPluginInfo().getSimulatorInfo();
            m_simulator = simulatorInfo;

            const QStringList names(internals.getSortedNames());
            if (names.isEmpty())
            {
                ui->tvp_Internals->clear();
                return;
            }

            // Update the INTERNAL view
            static const CIcon emptyIcon;
            const bool resize = true;
            const bool skipEqualValues = true;
            for (const QString &name : names)
            {
                ui->tvp_Internals->addOrUpdateByName(name, internals.getVariantValue(name), emptyIcon, resize, skipEqualValues);
            }
            ui->tvp_Internals->fullResizeToContents();
        }

        int CSimulatorComponent::getUpdateIntervalMs() const
        {
            // much slower updates via DBus
            if (!sGui || sGui->isShuttingDown()) { return 10000; }
            return sGui->getIContextSimulator()->isUsingImplementingObject() ? 1000 : 5000;
        }
    }
} // namespace
