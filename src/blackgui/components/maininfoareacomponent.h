/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MAININFOAREACOMPONENT_H
#define BLACKGUI_MAININFOAREACOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/infoarea.h"
#include <QObject>
#include <QScopedPointer>
#include <QSize>

class QPixmap;
class QWidget;

namespace Ui { class CMainInfoAreaComponent; }
namespace BlackGui::Components
{
    // forward declaration to break compile dependency from all components
    class CAircraftComponent;
    class CAtcStationComponent;
    class CCockpitComponent;
    class CFlightPlanComponent;
    class CLogComponent;
    class CMappingComponent;
    class CInterpolationComponent;
    class CRadarComponent;
    class CSettingsComponent;
    class CSimulatorComponent;
    class CTextMessageComponent;
    class CUserComponent;
    class CWeatherComponent;

    //! Main info area of pilot client
    class BLACKGUI_EXPORT CMainInfoAreaComponent : public BlackGui::CInfoArea
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CMainInfoAreaComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CMainInfoAreaComponent() override;

        //! Info areas
        enum InfoArea
        {
            // index must match tab index!
            InfoAreaCockpit       = 0,
            InfoAreaAircraft      = 1,
            InfoAreaAtc           = 2,
            InfoAreaUsers         = 3,
            InfoAreaTextMessages  = 4,
            InfoAreaSimulator     = 5,
            InfoAreaFlightPlan    = 6,
            InfoAreaWeather       = 7,
            InfoAreaMapping       = 8, //!< aka rendering, models
            InfoAreaInterpolation = 9,
            InfoAreaSettings      = 10,
            InfoAreaLog           = 11,
            InfoAreaRadar         = 12,
            InfoAreaNone          = -1
        };

        //! Cockpit
        CCockpitComponent *getCockpitComponent();

        //! ATC stations
        CAtcStationComponent *getAtcStationComponent();

        //! Aircrafts
        CAircraftComponent *getAircraftComponent();

        //! Mappings
        CMappingComponent *getMappingComponent();

        //! Interpolation
        CInterpolationComponent *getInterpolationComponent();

        //! User component
        CUserComponent *getUserComponent();

        //! Flight plan
        CFlightPlanComponent *getFlightPlanComponent();

        //! Settings
        CSettingsComponent *getSettingsComponent();

        //! Weather component
        CWeatherComponent *getWeatherComponent();

        //! Log messages
        CLogComponent *getLogComponent();

        //! Simulator
        CSimulatorComponent *getSimulatorComponent();

        //! Text messages
        CTextMessageComponent *getTextMessageComponent();

        //! Radar
        CRadarComponent *getRadarComponent();

        //! Display the log
        void displayLog();

        //! Display console
        void displayConsole();

        //! Toggle floating of given area
        void toggleFloating(InfoArea infoArea) { CInfoArea::toggleFloatingByIndex(static_cast<int>(infoArea)); }

        //! Select area
        void selectArea(InfoArea infoArea);

        //! Select log
        void selectLog();

        //! Select settings with given area
        void selectSettingsTab(int index);

        //! Select the audio tab
        void selectAudioTab();

    protected:
        // CInfoArea overrides
        virtual QSize getPreferredSizeWhenFloating(int areaIndex) const override;
        virtual const QPixmap &indexToPixmap(int areaIndex) const override;

    private:
        QScopedPointer<Ui::CMainInfoAreaComponent> ui;
    };
} // ns

#endif // guard
