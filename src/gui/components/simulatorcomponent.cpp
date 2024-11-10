// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "ui_simulatorcomponent.h"
#include "core/simulator.h"
#include "gui/components/simulatorcomponent.h"
#include "gui/views/namevariantpairview.h"
#include "core/context/contextownaircraft.h"
#include "core/context/contextsimulator.h"
#include "gui/guiapplication.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatorplugininfo.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/altitude.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/heading.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"
#include "misc/pq/angle.h"
#include "misc/pq/frequency.h"
#include "misc/pq/speed.h"
#include "misc/stringutils.h"
#include "misc/iconlist.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;
using namespace swift::core;
using namespace swift::core::context;

namespace swift::gui::components
{
    const QStringList &CSimulatorComponent::getLogCategories()
    {
        static const QStringList cats { CLogCategories::guiComponent(), CLogCategories::matching() };
        return cats;
    }

    CSimulatorComponent::CSimulatorComponent(QWidget *parent) : QTabWidget(parent),
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
        connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CSimulatorComponent::onSimulatorStatusChanged, Qt::QueuedConnection);
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
    {}

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

        const ISimulator::SimulatorStatus simulatorStatus = static_cast<ISimulator::SimulatorStatus>(sGui->getIContextSimulator()->getSimulatorStatus());
        if (simulatorStatus == ISimulator::Unspecified || simulatorStatus == ISimulator::Disconnected)
        {
            static const QString s("No simulator available");
            this->addOrUpdateLiveDataByName(QStringLiteral("info"), s, CIcons::StandardIconWarning16);
            return;
        }

        if (!simulatorStatus.testFlag(ISimulator::Simulating))
        {
            static const QString s("Simulator (%1) not yet running");
            this->addOrUpdateLiveDataByName(QStringLiteral("info"), s.arg(sGui->getIContextSimulator()->getSimulatorPluginInfo().getSimulator()), CIcons::StandardIconWarning16);
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
        static const CIcon iconPlane(CIcon::iconByIndex(CIcons::StandardIconPaperPlane16));

        if (m_simulator.isAnySimulator())
        {
            this->addOrUpdateLiveDataByName("simulator", m_simulator.toQString(true), m_simulator.toIcon());

            if (sGui->getISimulator())
            {
                const double fps = sGui->getISimulator()->getAverageFPS();
                this->addOrUpdateLiveDataByName(QStringLiteral("FPS"), fps < 0 ? QStringLiteral("N/A") : QString::number(fps, 'f', 1), CIcon(CIcons::ApplicationSimulator));

                const double ratio = sGui->getISimulator()->getSimTimeRatio();
                this->addOrUpdateLiveDataByName(QStringLiteral("Time Ratio"), QString::number(ratio, 'f', 2), CIcon(CIcons::ApplicationSimulator));

                const double miles = sGui->getISimulator()->getTrackMilesShort();
                this->addOrUpdateLiveDataByName(QStringLiteral("Miles Short"), QString::number(miles, 'f', 1), CIcon(CIcons::ApplicationSimulator));

                const double minutes = sGui->getISimulator()->getMinutesLate();
                this->addOrUpdateLiveDataByName(QStringLiteral("Minutes Late"), QString::number(minutes, 'f', 1), CIcon(CIcons::ApplicationSimulator));
            }
        }

        this->addOrUpdateLiveDataByName(QStringLiteral("latitude"), s.latitude().toQString(), iconLatLng);
        this->addOrUpdateLiveDataByName(QStringLiteral("longitude"), s.longitude().toQString(), iconLatLng);
        this->addOrUpdateLiveDataByName(QStringLiteral("altitude, true (ft)"), s.getAltitude().valueRoundedWithUnit(CLengthUnit::ft(), 1), iconAlt);
        this->addOrUpdateLiveDataByName(QStringLiteral("altitude, true (m)"), s.getAltitude().valueRoundedWithUnit(CLengthUnit::m(), 2), iconAlt);
        this->addOrUpdateLiveDataByName(QStringLiteral("altitude, pressure (ft)"), s.getPressureAltitude().valueRoundedWithUnit(CLengthUnit::ft(), 1), iconAlt);
        this->addOrUpdateLiveDataByName(QStringLiteral("altitude, pressure (m)"), s.getPressureAltitude().valueRoundedWithUnit(CLengthUnit::m(), 2), iconAlt);

        if (s.hasGroundElevation())
        {
            this->addOrUpdateLiveDataByName(QStringLiteral("elevation (ft)"), s.getGroundElevation().valueRoundedWithUnit(CLengthUnit::ft(), 1), iconAlt);
            this->addOrUpdateLiveDataByName(QStringLiteral("elevation (m)"), s.getGroundElevation().valueRoundedWithUnit(CLengthUnit::m(), 2), iconAlt);
        }
        else
        {
            this->addOrUpdateLiveDataByName(QStringLiteral("elevation"), QStringLiteral("N/A"), iconAlt);
        }

        if (ownAircraft.hasCG())
        {
            this->addOrUpdateLiveDataByName(QStringLiteral("CG (ft)"), ownAircraft.getCG().valueRoundedWithUnit(CLengthUnit::ft(), 1), iconPlane);
            this->addOrUpdateLiveDataByName(QStringLiteral("CG (m)"), ownAircraft.getCG().valueRoundedWithUnit(CLengthUnit::m(), 2), iconPlane);
        }
        else
        {
            this->addOrUpdateLiveDataByName(QStringLiteral("CG"), QStringLiteral("N/A"), iconPlane);
        }

        this->addOrUpdateLiveDataByName(QStringLiteral("pitch"), s.getPitch().toQString(), iconAttitude);
        this->addOrUpdateLiveDataByName(QStringLiteral("bank"), s.getBank().toQString(), iconAttitude);

        const CHeading heading = s.getHeading().normalizedTo360Degrees();
        this->addOrUpdateLiveDataByName(QStringLiteral("heading"), heading.valueRoundedWithUnit(CAngleUnit::deg(), 1), s.getHeading().toIcon());

        const CSpeed gs = s.getGroundSpeed();
        this->addOrUpdateLiveDataByName(QStringLiteral("ground speed (kts)"), gs.valueRoundedWithUnit(CSpeedUnit::kts(), 1), gs.toIcon());
        this->addOrUpdateLiveDataByName(QStringLiteral("ground speed (km/h)"), gs.valueRoundedWithUnit(CSpeedUnit::km_h(), 1), gs.toIcon());

        this->addOrUpdateLiveDataByName(QStringLiteral("COM1 active"), c1.getFrequencyActive().toQString(), iconRadio);
        this->addOrUpdateLiveDataByName(QStringLiteral("COM2 active"), c2.getFrequencyActive().toQString(), iconRadio);
        this->addOrUpdateLiveDataByName(QStringLiteral("COM1 standby"), c1.getFrequencyStandby().toQString(), iconRadio);
        this->addOrUpdateLiveDataByName(QStringLiteral("COM2 standby"), c2.getFrequencyStandby().toQString(), iconRadio);
        this->addOrUpdateLiveDataByName(QStringLiteral("COM1 volume"), QString::number(c1.getVolumeReceive()), iconRadio);
        this->addOrUpdateLiveDataByName(QStringLiteral("COM2 volume"), QString::number(c2.getVolumeReceive()), iconRadio);
        this->addOrUpdateLiveDataByName(QStringLiteral("Transponder"), ownAircraft.getTransponderCodeFormatted(), iconRadio);
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

    void CSimulatorComponent::onAddingRemoteModelFailed(const CSimulatedAircraft &aircraft, bool disabled, bool failover, const CStatusMessage &message)
    {
        const CStatusMessage msg = CStatusMessage(this).warning(u"Adding model '%1' failed, disabled: %2: failover: %3 details: %4")
                                   << aircraft.getModelString() << boolToYesNo(disabled) << boolToYesNo(failover) << aircraft.toQString(true);
        ui->comp_StatusMessages->appendStatusMessageToList(msg);
        ui->comp_StatusMessages->appendStatusMessageToList(message);
    }

    void CSimulatorComponent::onSimulatorMessages(const CStatusMessageList &messages)
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
} // namespace
