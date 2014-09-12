/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MAININFOAREACOMPONENT_H
#define BLACKGUI_MAININFOAREACOMPONENT_H

#include "../infoarea.h"
#include "atcstationcomponent.h"
#include "aircraftcomponent.h"
#include "usercomponent.h"
#include "textmessagecomponent.h"
#include "simulatorcomponent.h"
#include "flightplancomponent.h"
#include "settingscomponent.h"
#include "logcomponent.h"
#include <QTabBar>
#include <QPixmap>

namespace Ui { class CMainInfoAreaComponent; }

namespace BlackGui
{
    namespace Components
    {

        //! Main info area
        class CMainInfoAreaComponent : public BlackGui::CInfoArea
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CMainInfoAreaComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CMainInfoAreaComponent();

            //! Info areas
            enum InfoArea
            {
                // index must match tab index!
                InfoAreaAircrafts    = 0,
                InfoAreaAtc          = 1,
                InfoAreaUsers        = 2,
                InfoAreaTextMessages = 3,
                InfoAreaSimulator    = 4,
                InfoAreaFlightPlan   = 5,
                InfoAreaWeather      = 6,
                InfoAreaMappings     = 7,
                InfoAreaLog          = 8,
                InfoAreaSettings     = 9,
                InfoAreaNone         = -1
            };

            //! ATC stations
            CAtcStationComponent *getAtcStationComponent();

            //! Aircrafts
            CAircraftComponent *getAircraftComponent();

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

            //! Selected area of non floating areas
            InfoArea getSelectedInfoArea() const { return static_cast<InfoArea>(getSelectedInfoAreaIndex()); }

        public slots:
            //! Toggle floating of given area
            void toggleFloating(InfoArea infoArea) { CInfoArea::toggleFloating(static_cast<int>(infoArea)); }

            //! Select area
            void selectArea(InfoArea infoArea) { CInfoArea::selectArea(static_cast<int>(infoArea)); }

            //! Select settings with given area
            void selectSettingsTab(int index);

        protected:
            //! \copydoc CInfoArea::getPreferredSizeWhenFloating
            virtual QSize getPreferredSizeWhenFloating(int areaIndex) const override;

            //! \copydoc CInfoArea::indexToPixmap
            virtual const QPixmap &indexToPixmap(int areaIndex) const override;

        private:
            Ui::CMainInfoAreaComponent *ui = nullptr;

        };
    }
}

#endif // guard
