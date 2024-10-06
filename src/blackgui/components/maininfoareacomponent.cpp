// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/logcomponent.h"
#include "blackgui/components/maininfoareacomponent.h"
#include "blackgui/components/settingscomponent.h"
#include "blackmisc/icons.h"
#include "ui_maininfoareacomponent.h"

#include <QIcon>
#include <QGuiApplication>

using namespace BlackMisc;
using namespace BlackGui;

namespace BlackGui::Components
{
    CMainInfoAreaComponent::CMainInfoAreaComponent(QWidget *parent) : CInfoArea(parent),
                                                                      ui(new Ui::CMainInfoAreaComponent)
    {
        ui->setupUi(this);
        CInfoArea::initInfoArea(); // init base class
        this->setWindowIcon(CIcons::swift64());
        this->getLogComponent()->showFilterDialog();
        connect(this->getLogComponent(), &CLogComponent::requestAttention, this, &CMainInfoAreaComponent::selectLog);
    }

    CMainInfoAreaComponent::~CMainInfoAreaComponent()
    {}

    CAtcStationComponent *CMainInfoAreaComponent::getAtcStationComponent()
    {
        return ui->comp_AtcStations;
    }

    CAircraftComponent *CMainInfoAreaComponent::getAircraftComponent()
    {
        return ui->comp_Aircraft;
    }

    CMappingComponent *CMainInfoAreaComponent::getMappingComponent()
    {
        return ui->comp_Mappings;
    }

    CInterpolationComponent *CMainInfoAreaComponent::getInterpolationComponent()
    {
        return ui->comp_Interpolation;
    }

    CUserComponent *CMainInfoAreaComponent::getUserComponent()
    {
        return ui->comp_Users;
    }

    CCockpitComponent *CMainInfoAreaComponent::getCockpitComponent()
    {
        return ui->comp_Cockpit;
    }

    CFlightPlanComponent *CMainInfoAreaComponent::getFlightPlanComponent()
    {
        return ui->comp_FlightPlan;
    }

    CSettingsComponent *CMainInfoAreaComponent::getSettingsComponent()
    {
        return ui->comp_Settings;
    }

    CWeatherComponent *CMainInfoAreaComponent::getWeatherComponent()
    {
        return ui->comp_Weather;
    }

    CLogComponent *CMainInfoAreaComponent::getLogComponent()
    {
        return ui->comp_Log;
    }

    CSimulatorComponent *CMainInfoAreaComponent::getSimulatorComponent()
    {
        return ui->comp_Simulator;
    }

    CTextMessageComponent *CMainInfoAreaComponent::getTextMessageComponent()
    {
        return ui->comp_TextMessages;
    }

    CRadarComponent *CMainInfoAreaComponent::getRadarComponent()
    {
        return ui->comp_Radar;
    }

    void CMainInfoAreaComponent::displayLog()
    {
        ui->comp_Log->displayLog();
        this->selectArea(InfoAreaLog);
    }

    void CMainInfoAreaComponent::selectArea(CMainInfoAreaComponent::InfoArea infoArea)
    {
        CInfoArea::selectArea(static_cast<int>(infoArea));

        const Qt::KeyboardModifiers km = QGuiApplication::queryKeyboardModifiers();
        const bool shift = km.testFlag(Qt::ShiftModifier);
        if (!shift) { return; }

        // pressing shift will go to overview
        if (infoArea == InfoAreaSettings)
        {
            ui->comp_Settings->setSettingsOverviewTab();
        }
        else if (infoArea == InfoAreaAircraft)
        {
            ui->comp_Aircraft->setTab(CAircraftComponent::TabAircraftInRange);
        }
        else if (infoArea == InfoAreaAtc)
        {
            ui->comp_AtcStations->setTab(CAtcStationComponent::TabAtcOnline);
        }
        else if (infoArea == InfoAreaMapping)
        {
            ui->comp_Mappings->setTab(CMappingComponent::TabRenderedAircraft);
        }
    }

    void CMainInfoAreaComponent::selectLog()
    {
        this->selectArea(InfoAreaLog);
    }

    QSize CMainInfoAreaComponent::getPreferredSizeWhenFloating(int areaIndex) const
    {
        const InfoArea area = static_cast<InfoArea>(areaIndex);
        switch (area)
        {
        case InfoAreaCockpit:
        case InfoAreaAircraft:
        case InfoAreaAtc:
        case InfoAreaUsers:
        case InfoAreaLog:
        case InfoAreaSimulator:
        case InfoAreaMapping:
        case InfoAreaInterpolation:
        case InfoAreaSettings:
        case InfoAreaTextMessages:
        case InfoAreaWeather:
        case InfoAreaRadar:
            return QSize(600, 400);
        case InfoAreaFlightPlan:
            return QSize(800, 600);
        default:
            return QSize(600, 400);
        }
    }

    void CMainInfoAreaComponent::selectSettingsTab(int index)
    {
        this->selectArea(InfoAreaSettings);
        ui->comp_Settings->setTab(static_cast<CSettingsComponent::SettingTab>(index));
    }

    void CMainInfoAreaComponent::selectAudioTab()
    {
        this->getCockpitComponent()->showAudio();
        this->selectArea(CMainInfoAreaComponent::InfoAreaCockpit);
    }

    const QPixmap &CMainInfoAreaComponent::indexToPixmap(int areaIndex) const
    {
        const InfoArea area = static_cast<InfoArea>(areaIndex);
        switch (area)
        {
        case InfoAreaCockpit: return CIcons::appCockpit16();
        case InfoAreaUsers: return CIcons::appUsers16();
        case InfoAreaWeather: return CIcons::appWeather16();
        case InfoAreaAtc: return CIcons::appAtc16();
        case InfoAreaAircraft: return CIcons::appAircraft16();
        case InfoAreaSettings: return CIcons::appSettings16();
        case InfoAreaFlightPlan: return CIcons::appFlightPlan16();
        case InfoAreaSimulator: return CIcons::appSimulator16();
        case InfoAreaMapping: return CIcons::appMappings16();
        case InfoAreaLog: return CIcons::appLog16();
        case InfoAreaRadar: return CIcons::appRadar16();
        case InfoAreaTextMessages: return CIcons::appTextMessages16();
        case InfoAreaInterpolation: return CIcons::appInterpolation16();
        default: return CIcons::empty();
        }
    }
} // namespace
