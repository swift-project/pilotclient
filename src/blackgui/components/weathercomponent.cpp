/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
#include <QStringView>
#include <QStringBuilder>
#include <QStringLiteral>
#include <QPointer>

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
            COverlayMessagesFrameEnableForDockWidgetInfoArea(parent),
            CIdentifiable(this),
            ui(new Ui::CWeatherComponent)
        {
            ui->setupUi(this);
            m_coordinateDialog->showElevation(false);
            m_coordinateDialog->setReadOnly(ui->cb_UseOwnAcftPosition->isChecked());
            connect(ui->pb_SetPosition, &QPushButton::clicked, this, &CWeatherComponent::showCoordinateDialog);
            connect(ui->pb_Update,      &QPushButton::clicked, this, &CWeatherComponent::updateWeatherInformationForced);

            m_weatherScenarios = CWeatherGrid::getAllScenarios();
            for (const auto &scenario : std::as_const(m_weatherScenarios))
            {
                ui->cb_weatherScenario->addItem(scenario.getName(), QVariant::fromValue(scenario));
            }

            const CWeatherScenario scenario = m_weatherScenarioSetting.get();
            ui->cb_weatherScenario->setCurrentIndex(scenario.getIndex());
            ui->pb_ActivateWeather->setIcon(CIcons::metar());

            this->setupConnections();
            ui->lbl_Status->setText({});

            // hotkeys
            const QString swift(CGuiActionBindHandler::pathSwiftPilotClient());
            m_hotkeyBindings.append(CGuiActionBindHandler::bindButton(ui->pb_ActivateWeather, swift + "Weather/Toggle weather", true));
            m_hotkeyBindings.append(CGuiActionBindHandler::bindButton(ui->pb_ActivateWeather, swift + "Weather/Force CAVOK", true));

            // Set interval to 5 mins
            m_weatherUpdateTimer.setInterval(1000 * 60 * 5);

            // Call this method deferred in order to have the component fully initialized, e.g. object name set by the parent
            QPointer<CWeatherComponent> myself(this);
            QTimer::singleShot(1000, this, [ = ]
            {
                if (!myself) { return; }
                myself->updateWeatherInformation(true);
                myself->updateWeatherInfoLine();
            });
        }

        CWeatherComponent::~CWeatherComponent()
        { }

        bool CWeatherComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
        {
            CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
            const bool c = connect(this->getParentInfoArea(), &CInfoArea::changedInfoAreaTabBarIndex, this, &CWeatherComponent::infoAreaTabBarChanged, Qt::QueuedConnection);
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
            Q_UNUSED(index)
        }

        void CWeatherComponent::toggleUseOwnAircraftPosition(bool useOwnAircraftPosition)
        {
            m_lastOwnAircraftPosition = {};
            m_coordinateDialog->setReadOnly(useOwnAircraftPosition);
            if (useOwnAircraftPosition)
            {
                m_weatherUpdateTimer.start();
                ui->pb_SetPosition->setText("Show Position");
            }
            else
            {
                m_weatherUpdateTimer.stop();
                const CCoordinateGeodetic c;
                m_coordinateDialog->setCoordinate(c);
                ui->pb_SetPosition->setText("Select Position");
            }
            updateWeatherInformation(false);
        }

        void CWeatherComponent::toggleWeatherActivation()
        {
            if (!sGui || !sGui->getIContextSimulator()) { return; }
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
            this->updateWeatherInfoLine();
        }

        void CWeatherComponent::showCoordinateDialog()
        {
            m_coordinateDialog->show();
        }

        void CWeatherComponent::setWeatherScenario(int index)
        {
            if (index == -1) { return; }
            m_lastOwnAircraftPosition = {};
            const CWeatherScenario scenario = m_weatherScenarios[index];
            m_weatherScenarioSetting.set(scenario);
            this->updateWeatherInformation(false);
            this->updateWeatherInfoLine();
        }

        void CWeatherComponent::setCavok()
        {
            QPointer<CWeatherComponent> myself(this);
            for (int index = 0; index < m_weatherScenarios.size(); index++)
            {
                if (m_weatherScenarios[index].getIndex() == CWeatherScenario::ClearSky)
                {
                    // call queued
                    QTimer::singleShot(0, this, [ = ] { if (myself) { myself->setWeatherScenario(index); }});
                    break;
                }
            }
        }

        void CWeatherComponent::updateWeatherInfoLine()
        {
            if (m_isWeatherActivated)
            {
                const CWeatherScenario scenario = m_weatherScenarioSetting.get();
                ui->lbl_WeatherEngineInfo->setText(QStringLiteral("swift weather is on, '%1'").arg(scenario.getName()));
            }
            else
            {
                ui->lbl_WeatherEngineInfo->setText(QStringLiteral("swift weather is off"));
            }
        }

        void CWeatherComponent::updateWeatherInformation(bool forceRealWeatherReload)
        {
            setWeatherGrid({});
            ui->lbl_Status->setText({});
            const bool useOwnAcftPosition = ui->cb_UseOwnAcftPosition->isChecked();
            CCoordinateGeodetic position;
            Q_ASSERT(ui->cb_weatherScenario->currentData().canConvert<CWeatherScenario>());
            const CWeatherScenario scenario = ui->cb_weatherScenario->currentData().value<CWeatherScenario>();

            if (useOwnAcftPosition)
            {
                Q_ASSERT(sGui->getIContextOwnAircraft());
                position = sGui->getIContextOwnAircraft()->getOwnAircraft().getPosition();
                m_coordinateDialog->setCoordinate(position);
            }
            else
            {
                position = m_coordinateDialog->getCoordinate();
                if (position.isNull())
                {
                    ui->lbl_Status->setText("No position selected.");
                    return;
                }
            }

            if (CWeatherScenario::isRealWeatherScenario(scenario))
            {
                if (m_lastOwnAircraftPosition.isNull() || forceRealWeatherReload ||
                        calculateGreatCircleDistance(position, m_lastOwnAircraftPosition).value(CLengthUnit::km()) > 20)
                {
                    this->requestWeatherGrid(position);
                    m_lastOwnAircraftPosition = position;
                    showOverlayHTMLMessage("Weather loading,<br>this may take a while", 7500);
                }
            }
            else
            {
                setWeatherGrid(CWeatherGrid::getByScenario(scenario));
            }
        }

        void CWeatherComponent::onWeatherGridReceived(const CWeatherGrid &weatherGrid, const CIdentifier &identifier)
        {
            if (!isMyIdentifier(identifier))
            {
                // not from myself
                const CWeatherScenario scenario = ui->cb_weatherScenario->currentData().value<CWeatherScenario>();
                if (!CWeatherScenario::isRealWeatherScenario(scenario)) { return; }

                // we have received weather grid data and assume those are real weather updates
                closeOverlay();
            }
            ui->lbl_Status->setText({});
            setWeatherGrid(weatherGrid);
        }

        void CWeatherComponent::setupConnections()
        {
            // UI connections
            connect(m_coordinateDialog.data(), &CCoordinateDialog::changedCoordinate,           this, &CWeatherComponent::updateWeatherInformationForced);
            connect(&m_weatherUpdateTimer,     &QTimer::timeout,      this, &CWeatherComponent::updateWeatherInformationChecked);
            connect(ui->cb_weatherScenario,    qOverload<int>(&QComboBox::currentIndexChanged), this, &CWeatherComponent::setWeatherScenario);
            connect(ui->cb_UseOwnAcftPosition, &QCheckBox::toggled,   this, &CWeatherComponent::toggleUseOwnAircraftPosition);
            connect(ui->pb_ActivateWeather,    &QPushButton::clicked, this, &CWeatherComponent::toggleWeatherActivation);

            // Context connections
            Q_ASSERT(sGui->getIContextSimulator());
            connect(sGui->getIContextSimulator(), &IContextSimulator::weatherGridReceived, this, &CWeatherComponent::onWeatherGridReceived, Qt::QueuedConnection);
        }

        void CWeatherComponent::setWeatherGrid(const CWeatherGrid &weatherGrid)
        {
            const CGridPoint gridPoint = weatherGrid.frontOrDefault();
            ui->tvp_TemperatureLayers->updateContainer(gridPoint.getTemperatureLayers());
            ui->tvp_CloudLayers->updateContainer(gridPoint.getCloudLayers());
            ui->tvp_WindLayers->updateContainer(gridPoint.getWindLayers());
            const CCoordinateGeodetic position = gridPoint.getPosition();
            const double pressureAtMsl = gridPoint.getPressureAtMsl().value(CPressureUnit::hPa());
            const QString status = QStringLiteral("Weather Position: %1 %2\nPressure (MSL): %3 hPa").arg(position.latitude().toWgs84(), position.longitude().toWgs84()).arg(pressureAtMsl);
            ui->lbl_Status->setText(status);
        }

        void CWeatherComponent::requestWeatherGrid(const CCoordinateGeodetic &position)
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }
            ui->lbl_Status->setText(QStringLiteral("Loading around %1 %2").arg(position.latitude().toWgs84(), position.longitude().toWgs84()));
            sGui->getIContextSimulator()->requestWeatherGrid(position, this->identifier());
        }

        void CWeatherComponent::onScenarioChanged()
        {
            this->updateWeatherInfoLine();
        }

    } // namespace
} // namespace
