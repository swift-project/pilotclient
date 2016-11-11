/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_WEATHERCOMPONENT_H
#define BLACKGUI_WEATHERCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackcore/simulator/simulatorsettings.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/weather/weatherscenario.h"

#include <QDateTime>
#include <QModelIndex>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QTimer>
#include <QWidget>
#include <QtGlobal>

namespace BlackMisc { namespace Weather { class CWeatherGrid; } }
namespace Ui { class CWeatherComponent; }

namespace BlackGui
{
    class CDockWidgetInfoArea;

    namespace Components
    {
        //! Weather component
        class BLACKGUI_EXPORT CWeatherComponent :
            public QWidget,
            public CEnableForDockWidgetInfoArea,
            public BlackMisc::CIdentifiable
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CWeatherComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CWeatherComponent();

            //! \copydoc CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
            virtual bool setParentDockWidgetInfoArea(BlackGui::CDockWidgetInfoArea *parentDockableWidget) override;

        private slots:
            void ps_infoAreaTabBarChanged(int index);

        private:
            void toggleUseOwnAircraftPosition(bool checked);
            void setWeatherScenario(int index);

            void updateWeatherInformation();
            void weatherGridReceived(const BlackMisc::Weather::CWeatherGrid &weatherGrid, const BlackMisc::CIdentifier &identifier);

            void setupConnections();
            void setupInputValidators();
            void setupCompleter();

            void setWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid);
            void requestWeatherGrid(const BlackMisc::Geo::CCoordinateGeodetic &position);

            QScopedPointer<Ui::CWeatherComponent> ui;
            QVector<BlackMisc::Weather::CWeatherScenario> m_weatherScenarios;
            QTimer m_weatherUpdateTimer { this };
            BlackMisc::Geo::CCoordinateGeodetic m_lastOwnAircraftPosition;
            BlackMisc::CSetting<BlackCore::Simulator::TSelectedWeatherScenario> m_weatherScenarioSetting { this };
        };
    } // namespace
} // namespace
#endif // guard
