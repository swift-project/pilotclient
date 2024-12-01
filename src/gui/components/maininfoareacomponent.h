// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MAININFOAREACOMPONENT_H
#define SWIFT_GUI_MAININFOAREACOMPONENT_H

#include <QObject>
#include <QScopedPointer>
#include <QSize>

#include "gui/infoarea.h"
#include "gui/swiftguiexport.h"

class QPixmap;
class QWidget;

namespace Ui
{
    class CMainInfoAreaComponent;
}
namespace swift::gui::components
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

    //! Main info area of pilot client
    class SWIFT_GUI_EXPORT CMainInfoAreaComponent : public swift::gui::CInfoArea
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
            InfoAreaCockpit = 0,
            InfoAreaAircraft = 1,
            InfoAreaAtc = 2,
            InfoAreaUsers = 3,
            InfoAreaTextMessages = 4,
            InfoAreaSimulator = 5,
            InfoAreaFlightPlan = 6,
            InfoAreaMapping = 7, //!< aka rendering, models
            InfoAreaInterpolation = 8,
            InfoAreaSettings = 9,
            InfoAreaLog = 10,
            InfoAreaRadar = 11,
            InfoAreaNone = -1
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
} // namespace swift::gui::components

#endif // SWIFT_GUI_MAININFOAREACOMPONENT_H
