/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/weathercomponent.h"
#include "blackgui/infoarea.h"
#include "blackgui/views/viewbase.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiactionbind.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/weather/weathergrid.h"
#include "ui_weathercomponent.h"

#include <QCompleter>
#include <QRegExpValidator>
#include <QRegularExpression>

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackGui;
using namespace BlackGui::Models;
using namespace BlackGui::Views;
using namespace BlackMisc;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Weather;

namespace BlackGui
{
    namespace Components
    {
        CWeatherComponent::CWeatherComponent(QWidget *parent) :
            QWidget(parent),
            CIdentifiable(this),
            ui(new Ui::CWeatherComponent)
        {
            ui->setupUi(this);
            m_coordinateDialog->showElevation(false);
            m_coordinateDialog->setReadOnly(ui->cb_UseOwnAcftPosition->isChecked());
            connect(ui->pb_Coordinate, &QPushButton::clicked, this, &CWeatherComponent::showCoordinateDialog);

            m_weatherScenarios = CWeatherGrid::getAllScenarios();
            for (const auto &scenario : as_const(m_weatherScenarios))
            {
                ui->cb_weatherScenario->addItem(scenario.getName(), QVariant::fromValue(scenario));
            }

            const auto scenario = m_weatherScenarioSetting.get();
            ui->cb_weatherScenario->setCurrentIndex(scenario.getIndex());
            ui->pb_ActivateWeather->setIcon(CIcons::metar());

            setupConnections();
            ui->lbl_Status->setText({});

            // hotkeys
            const QString swift(CGuiActionBindHandler::pathSwiftPilotClient());
            m_hotkeyBindings.append(CGuiActionBindHandler::bindButton(ui->pb_ActivateWeather, swift + "Weather/Toggle weather", true));
            m_hotkeyBindings.append(CGuiActionBindHandler::bindButton(ui->pb_ActivateWeather, swift + "Weather/Force CAVOK", true));

            // Set interval to 5 min
            m_weatherUpdateTimer.setInterval(1000 * 60 * 5);

            // Call this method deferred in order to have the component fully initialized, e.g. object name set by the parent
            QTimer::singleShot(1000, this, &CWeatherComponent::updateWeatherInformation);
        }

        CWeatherComponent::~CWeatherComponent()
        { }

        bool CWeatherComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
        {
            CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
            bool c = connect(this->getParentInfoArea(), &CInfoArea::changedInfoAreaTabBarIndex, this, &CWeatherComponent::infoAreaTabBarChanged);
            Q_ASSERT_X(c, Q_FUNC_INFO, "failed connect");
            Q_ASSERT_X(parentDockableWidget, Q_FUNC_INFO, "missing parent");
            return c && parentDockableWidget;
        }

        void CWeatherComponent::infoAreaTabBarChanged(int index)
        {
            // ignore in those cases
            if (!this->isVisibleWidget()) { return; }
            if (this->isParentDockWidgetFloating()) { return; }

            // here I know I am the selected widget, update, but keep GUI responsive (-> timer)
            //QTimer::singleShot(1000, this, &CWeatherComponent::update);
            Q_UNUSED(index);
        }

        void CWeatherComponent::toggleUseOwnAircraftPosition(bool useOwnAircraftPosition)
        {
            m_lastOwnAircraftPosition = {};
            m_coordinateDialog->setReadOnly(useOwnAircraftPosition);
            if (useOwnAircraftPosition)
            {
                m_weatherUpdateTimer.start();
            }
            else
            {
                m_weatherUpdateTimer.stop();
            }
            updateWeatherInformation();
        }

        void CWeatherComponent::toggleWeatherActivation()
        {
            if (m_isWeatherActivated)
            {
                m_isWeatherActivated = false;
                ui->pb_ActivateWeather->setText("Activate");
            }
            else
            {
                m_isWeatherActivated = true;
                ui->pb_ActivateWeather->setText("Deactivate");
            }
            sGui->getIContextSimulator()->setWeatherActivated(m_isWeatherActivated);
        }

        void CWeatherComponent::showCoordinateDialog()
        {
            m_coordinateDialog->show();
        }

        void CWeatherComponent::setWeatherScenario(int index)
        {
            if (index == -1) { return; }
            m_lastOwnAircraftPosition = {};
            CWeatherScenario scenario = m_weatherScenarios[index];
            m_weatherScenarioSetting.set(scenario);
            updateWeatherInformation();

        }

        void CWeatherComponent::setCavok()
        {
            for (int index = 0; index < m_weatherScenarios.size(); index++)
            {
                if (m_weatherScenarios[index].getIndex() == CWeatherScenario::ClearSky)
                {
                    // call queued
                    QTimer::singleShot(0, this, [ = ] { this->setWeatherScenario(index); });
                    break;
                }
            }
        }

        void CWeatherComponent::updateWeatherInformation()
        {
            setWeatherGrid({});
            const bool useOwnAcftPosition = ui->cb_UseOwnAcftPosition->isChecked();
            BlackMisc::Geo::CCoordinateGeodetic position;
            Q_ASSERT(ui->cb_weatherScenario->currentData().canConvert<CWeatherScenario>());
            CWeatherScenario scenario = ui->cb_weatherScenario->currentData().value<CWeatherScenario>();

            if (useOwnAcftPosition)
            {
                Q_ASSERT(sGui->getIContextOwnAircraft());
                position = sGui->getIContextOwnAircraft()->getOwnAircraft().getPosition();
            }
            else
            {
                position = m_coordinateDialog->getCoordinate();
            }

            if (CWeatherScenario::isRealWeatherScenario(scenario))
            {
                if (m_lastOwnAircraftPosition.isNull() ||
                        calculateGreatCircleDistance(position, m_lastOwnAircraftPosition).value(CLengthUnit::km()) > 20)
                {
                    requestWeatherGrid(position);
                    m_lastOwnAircraftPosition = position;
                }
            }
            else
            {
                setWeatherGrid(CWeatherGrid::getByScenario(scenario));
            }
        }

        void CWeatherComponent::weatherGridReceived(const CWeatherGrid &weatherGrid, const CIdentifier &identifier)
        {
            if (!isMyIdentifier(identifier)) { return; }
            ui->lbl_Status->setText({});
            setWeatherGrid(weatherGrid);
        }

        void CWeatherComponent::setupConnections()
        {
            // UI connections
            connect(ui->cb_weatherScenario, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                    this, &CWeatherComponent::setWeatherScenario);
            connect(m_coordinateDialog.data(), &CCoordinateDialog::changedCoordinate, this, &CWeatherComponent::updateWeatherInformation);
            connect(ui->cb_UseOwnAcftPosition, &QCheckBox::toggled, this, &CWeatherComponent::toggleUseOwnAircraftPosition);
            connect(&m_weatherUpdateTimer, &QTimer::timeout, this, &CWeatherComponent::updateWeatherInformation);
            connect(ui->pb_ActivateWeather, &QPushButton::clicked, this, &CWeatherComponent::toggleWeatherActivation);

            // Context connections
            Q_ASSERT(sGui->getIContextSimulator());
            connect(sGui->getIContextSimulator(), &IContextSimulator::weatherGridReceived, this, &CWeatherComponent::weatherGridReceived);
        }

        void CWeatherComponent::setWeatherGrid(const CWeatherGrid &weatherGrid)
        {
            auto gridPoint = weatherGrid.frontOrDefault();
            ui->tvp_TemperatureLayers->updateContainer(gridPoint.getTemperatureLayers());
            ui->tvp_CloudLayers->updateContainer(gridPoint.getCloudLayers());
            ui->tvp_WindLayers->updateContainer(gridPoint.getWindLayers());
        }

        void CWeatherComponent::requestWeatherGrid(const CCoordinateGeodetic &position)
        {
            ui->lbl_Status->setText("Loading...");
            CWeatherGrid weatherGrid { { {}, position } };
            auto ident = identifier();
            sGui->getIContextSimulator()->requestWeatherGrid(weatherGrid, ident);
        }
    } // namespace
} // namespace
