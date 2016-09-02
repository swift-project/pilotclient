/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/simulator.h"
#include "blackgui/components/simulatorcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/views/namevariantpairview.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "ui_simulatorcomponent.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackCore::Context;

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
            ui->tvp_LiveData->setIconMode(true);
            ui->tvp_LiveData->setAutoResizeFrequency(10); // only resize every n-th time
            this->addOrUpdateByName("info", "no data yet", CIcons::StandardIconWarning16);

            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CSimulatorComponent::ps_onSimulatorStatusChanged);
            this->setUpdateInterval(getUpdateIntervalMs());
            if (sGui->getIContextSimulator()->getSimulatorStatus() == 0)
            {
                this->stopTimer();
            }
        }

        CSimulatorComponent::~CSimulatorComponent()
        { }

        void CSimulatorComponent::addOrUpdateByName(const QString &name, const QString &value, const CIcon &icon)
        {
            bool resize = this->currentWidget() == ui->tb_LiveData; // simulator live data selected?
            ui->tvp_LiveData->addOrUpdateByName(name, value, icon, resize, false);
        }

        void CSimulatorComponent::addOrUpdateByName(const QString &name, const QString &value, CIcons::IconIndex iconIndex)
        {
            this->addOrUpdateByName(name, value, CIconList::iconByIndex(iconIndex));
        }

        int CSimulatorComponent::rowCount() const
        {
            return ui->tvp_LiveData->rowCount();
        }

        void CSimulatorComponent::clear()
        {
            ui->tvp_LiveData->clear();
        }

        void CSimulatorComponent::update()
        {
            if (!this->isVisibleWidget()) return; // no updates on invisible widgets
            if (!sGui->getIContextOwnAircraft()) return;

            int simualtorStatus = sGui->getIContextSimulator()->getSimulatorStatus();
            if (simualtorStatus == 0)
            {
                addOrUpdateByName("info", tr("No simulator available"), CIcons::StandardIconWarning16);
                return;
            }

            if (!(simualtorStatus & ISimulator::Simulating))
            {
                this->addOrUpdateByName("info",
                                        tr("Simulator (%1) not yet running").arg(
                                            sGui->getIContextSimulator()->getSimulatorPluginInfo().getSimulator()
                                        ),
                                        CIcons::StandardIconWarning16);
                return;
            }

            // clear old warnings / information
            if (this->rowCount() < 5)
            {
                this->clear();
            }

            CSimulatedAircraft ownAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
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
            // much slower updates via DBus
            return sGui->getIContextSimulator()->isUsingImplementingObject() ? 500 : 5000;
        }
    }
} // namespace
