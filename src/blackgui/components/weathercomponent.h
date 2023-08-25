// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_WEATHERCOMPONENT_H
#define BLACKGUI_WEATHERCOMPONENT_H

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/components/coordinatedialog.h"
#include "blackgui/blackguiexport.h"
#include "blackcore/actionbind.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/weather/weatherscenario.h"
#include "blackmisc/identifiable.h"

#include <QDateTime>
#include <QModelIndex>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QTimer>
#include <QWidget>
#include <QtGlobal>

namespace BlackMisc::Weather
{
    class CWeatherGrid;
}
namespace Ui
{
    class CWeatherComponent;
}

namespace BlackGui
{
    class CDockWidgetInfoArea;

    namespace Components
    {
        //! Weather component
        class BLACKGUI_EXPORT CWeatherComponent :
            public COverlayMessagesFrameEnableForDockWidgetInfoArea,
            public BlackMisc::CIdentifiable
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CWeatherComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CWeatherComponent() override;

            //! \copydoc CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
            virtual bool setParentDockWidgetInfoArea(BlackGui::CDockWidgetInfoArea *parentDockableWidget) override;

        private:
            void infoAreaTabBarChanged(int index);

            void toggleUseOwnAircraftPosition(bool useOwnAircraftPosition);
            void toggleWeatherActivation();
            void showCoordinateDialog();
            void setWeatherScenario(int index);
            void setCavok();

            void updateWeatherInfoLine();
            void updateWeatherInformationForced() { this->updateWeatherInformation(true); }
            void updateWeatherInformationChecked() { this->updateWeatherInformation(false); }
            void updateWeatherInformation(bool forceRealWeatherReload);
            void onWeatherGridReceived(const BlackMisc::Weather::CWeatherGrid &weatherGrid, const BlackMisc::CIdentifier &identifier);

            void setupConnections();

            void setWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid);
            void requestWeatherGrid(const BlackMisc::Geo::CCoordinateGeodetic &position);

            void onScenarioChanged();

            QScopedPointer<Ui::CWeatherComponent> ui;
            QScopedPointer<CCoordinateDialog> m_coordinateDialog { new CCoordinateDialog(this) };
            QVector<BlackMisc::Weather::CWeatherScenario> m_weatherScenarios;
            QTimer m_weatherUpdateTimer; //!< this is the timer for weather updates
            BlackMisc::Geo::CCoordinateGeodetic m_lastOwnAircraftPosition;
            BlackMisc::CSetting<BlackMisc::Simulation::Settings::TSelectedWeatherScenario> m_weatherScenarioSetting { this, &CWeatherComponent::onScenarioChanged };
            BlackCore::CActionBindings m_hotkeyBindings; //!< allow binding of hotkey
            bool m_isWeatherActivated = false;
        };
    } // namespace
} // namespace
#endif // guard
