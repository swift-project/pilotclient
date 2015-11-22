/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorcomponent.h"
#include "ui_simulatorcomponent.h"
#include "blackcore/contextsimulator.h"
#include "blackcore/contextownaircraft.h"
#include "blackcore/simulator.h"
#include "blackmisc/iconlist.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CSimulatorComponent::CSimulatorComponent(QWidget *parent) :
            QTabWidget(parent),
            CEnableForDockWidgetInfoArea(),
            ui(new Ui::CSimulatorComponent),
            m_updateTimer(new CUpdateTimer("CSimulatorComponent", &CSimulatorComponent::update, this))
        {
            ui->setupUi(this);
            this->ui->tvp_LiveData->setIconMode(true);
            this->ui->tvp_LiveData->setAutoResizeFrequency(10); // only resize every n-th time
            this->addOrUpdateByName("info", "no data yet", CIcons::StandardIconWarning16);
        }

        CSimulatorComponent::~CSimulatorComponent()
        { }

        void CSimulatorComponent::addOrUpdateByName(const QString &name, const QString &value, const CIcon &icon)
        {
            bool resize = this->currentWidget() == this->ui->tb_LiveData; // simulator live data selected?
            this->ui->tvp_LiveData->addOrUpdateByName(name, value, icon, resize, false);
        }

        void CSimulatorComponent::addOrUpdateByName(const QString &name, const QString &value, CIcons::IconIndex iconIndex)
        {
            this->addOrUpdateByName(name, value, CIconList::iconByIndex(iconIndex));
        }

        int CSimulatorComponent::rowCount() const
        {
            return this->ui->tvp_LiveData->rowCount();
        }

        void CSimulatorComponent::clear()
        {
            this->ui->tvp_LiveData->clear();
        }

        void CSimulatorComponent::update()
        {
            Q_ASSERT_X(getIContextSimulator(), Q_FUNC_INFO, "No simulator context");

            if (!this->isVisibleWidget()) return; // no updates on invisible widgets
            if (!this->getIContextOwnAircraft()) return;

            int simualtorStatus = this->getIContextSimulator()->getSimulatorStatus();
            if (simualtorStatus == 0)
            {
                addOrUpdateByName("info", tr("No simulator available"), CIcons::StandardIconWarning16);
                return;
            }

            if (!(simualtorStatus & ISimulator::Simulating))
            {
                this->addOrUpdateByName("info",
                                        tr("Simulator (%1) not yet running").arg(
                                            getIContextSimulator()->getSimulatorPluginInfo().getSimulator()
                                        ),
                                        CIcons::StandardIconWarning16);
                return;
            }

            // clear old warnings / information
            if (this->rowCount() < 5)
            {
                this->clear();
            }

            CSimulatedAircraft ownAircraft = this->getIContextOwnAircraft()->getOwnAircraft();
            CAircraftSituation s = ownAircraft.getSituation();
            CComSystem c1 = ownAircraft.getCom1System();
            CComSystem c2 = ownAircraft.getCom2System();

            this->addOrUpdateByName("latitude", s.latitude().toFormattedQString(), s.latitude().toIcon());
            this->addOrUpdateByName("longitude", s.longitude().toFormattedQString(), s.longitude().toIcon());
            this->addOrUpdateByName("altitude", s.getAltitude().toFormattedQString(), s.getAltitude().toIcon());
            this->addOrUpdateByName("pitch", s.getPitch().toFormattedQString(), CIcons::AviationAttitudeIndicator);
            this->addOrUpdateByName("bank", s.getBank().toFormattedQString(), CIcons::AviationAttitudeIndicator);
            this->addOrUpdateByName("heading", s.getHeading().toFormattedQString(), s.getHeading().toIcon());
            this->addOrUpdateByName("ground speed", s.getGroundSpeed().toFormattedQString(), s.getGroundSpeed().toIcon());

            this->addOrUpdateByName("COM1 active", c1.getFrequencyActive().toFormattedQString(), CIcons::StandardIconRadio16);
            this->addOrUpdateByName("COM2 active", c2.getFrequencyActive().toFormattedQString(), CIcons::StandardIconRadio16);
            this->addOrUpdateByName("COM1 standby", c1.getFrequencyStandby().toFormattedQString(), CIcons::StandardIconRadio16);
            this->addOrUpdateByName("COM2 standby", c2.getFrequencyStandby().toFormattedQString(), CIcons::StandardIconRadio16);
            this->addOrUpdateByName("Transponder", ownAircraft.getTransponderCodeFormatted(), CIcons::StandardIconRadio16);
        }

        void CSimulatorComponent::runtimeHasBeenSet()
        {
            Q_ASSERT_X(this->getIContextSimulator(), Q_FUNC_INFO, "Missing simulator context");
            if (!this->getIContextSimulator()) { return; }
            QObject::connect(this->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CSimulatorComponent::ps_onSimulatorStatusChanged);

            this->setUpdateInterval(getUpdateIntervalMs());
            if (getIContextSimulator()->getSimulatorStatus() == 0)
            {
                this->stopTimer();
            }
        }

        void CSimulatorComponent::ps_onSimulatorStatusChanged(int status)
        {
            if (status & ISimulator::Connected)
            {
                int intervalMs = getUpdateIntervalMs();
                this->m_updateTimer->startTimer(intervalMs);
            }
            else
            {
                this->stopTimer();
                clear();
                update();
            }
        }

        int CSimulatorComponent::getUpdateIntervalMs() const
        {
            Q_ASSERT(this->getIContextSimulator());

            // much slower updates via DBus
            return this->getIContextSimulator()->isUsingImplementingObject() ? 500 : 5000;
        }
    }
} // namespace
