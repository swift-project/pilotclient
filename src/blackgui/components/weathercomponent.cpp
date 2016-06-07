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
#include "blackcore/contextapplication.h"
#include "blackcore/contextsimulator.h"
#include "blackcore/contextownaircraft.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/weather/weathergrid.h"
#include "ui_weathercomponent.h"

#include <QCompleter>
#include <QRegExpValidator>
#include <QRegularExpression>

using namespace BlackCore;
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
            m_weatherScenarios = CWeatherGrid::getAllScenarios();

            for (const auto &scenario : m_weatherScenarios)
            {
                ui->cb_weatherScenario->addItem(scenario.getName(), QVariant::fromValue(scenario));
            }
            auto scenario = m_weatherScenarioSetting.get();
            ui->cb_weatherScenario->setCurrentIndex(scenario.getIndex());

            setupConnections();
            setupInputValidators();
            setupCompleter();

            // Set interval to 5 min
            m_weatherUpdateTimer.setInterval(1000 * 60 * 5);

            // Call this method deferred in order to have the component fully initialized, e.g. object name set by the parent
            QTimer::singleShot(0, this, &CWeatherComponent::updateWeatherInformation);
        }

        CWeatherComponent::~CWeatherComponent()
        { }

        bool CWeatherComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
        {
            CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
            bool c = connect(this->getParentInfoArea(), &CInfoArea::changedInfoAreaTabBarIndex, this, &CWeatherComponent::ps_infoAreaTabBarChanged);
            Q_ASSERT_X(c, Q_FUNC_INFO, "failed connect");
            Q_ASSERT_X(parentDockableWidget, Q_FUNC_INFO, "missing parent");
            return c && parentDockableWidget;
        }

        void CWeatherComponent::ps_infoAreaTabBarChanged(int index)
        {
            // ignore in those cases
            if (!this->isVisibleWidget()) { return; }
            if (this->isParentDockWidgetFloating()) { return; }

            // here I know I am the selected widget, update, but keep GUI responsive (-> timer)
            //QTimer::singleShot(1000, this, &CWeatherComponent::update);
            Q_UNUSED(index);
        }

        void CWeatherComponent::toggleUseOwnAircraftPosition(bool checked)
        {
            m_lastOwnAircraftPosition = {};
            if (checked)
            {
                ui->le_LatOrIcao->setReadOnly(true);
                ui->le_Lon->setReadOnly(true);
                m_weatherUpdateTimer.start();
                updateWeatherInformation();
            }
            else
            {
                m_weatherUpdateTimer.stop();
                ui->le_LatOrIcao->setReadOnly(false);
                ui->le_LatOrIcao->setText({});
                ui->le_Lon->setReadOnly(false);
                ui->le_Lon->setText({});
                updateWeatherInformation();
            }
        }

        void CWeatherComponent::setWeatherScenario(int index)
        {
            if (index == -1) { return; }
            m_lastOwnAircraftPosition = {};
            auto scenario = m_weatherScenarios[index];
            m_weatherScenarioSetting.set(scenario);
            updateWeatherInformation();

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
                if(position == CCoordinateGeodetic())
                {
                    ui->le_LatOrIcao->setText("N/A");
                    ui->le_Lon->setText("N/A");
                    return;
                }
                ui->le_LatOrIcao->setText(position.latitude().toQString());
                ui->le_Lon->setText(position.longitude().toQString());


            }
            else
            {
                QString latitudeOrIcao = ui->le_LatOrIcao->text();
                QString longitude = ui->le_Lon->text();
                const QRegularExpression reIcao("^[a-zA-Z]{4}$", QRegularExpression::CaseInsensitiveOption);
                auto icaoMatch = reIcao.match(latitudeOrIcao);
                if (icaoMatch.hasMatch())
                {
                    CLogMessage(this).warning("Requested weather for ICAO station %1. Unfortunately ICAO support is not yet implemented.") << latitudeOrIcao;
                    return;
                }

                const QRegularExpression reDecimalNumber("^-?\\d{1,2}[,.]?\\d+$", QRegularExpression::CaseInsensitiveOption);
                auto latitudeMatch = reDecimalNumber.match(latitudeOrIcao);
                auto longitudeMatch = reDecimalNumber.match(longitude);
                if (!latitudeMatch.hasMatch() || !longitudeMatch.hasMatch())
                {
                    CLogMessage(this).warning("Invalid position - Latitude: %1, Longitude: %2") << latitudeOrIcao << longitude;
                    return;
                }

                // Replace ',' with '.'. Both are allowed as input, but QString::toDouble() only accepts '.'
                latitudeOrIcao = latitudeOrIcao.replace(',', '.');
                longitude = longitude.replace(',', '.');
                position = { latitudeOrIcao.toDouble(), longitude.toDouble(), 0 };
            }

            if (CWeatherScenario::isRealWeatherScenario(scenario))
            {
                if (!useOwnAcftPosition ||
                        calculateGreatCircleDistance(position, m_lastOwnAircraftPosition).value(CLengthUnit::km()) > 20 )
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
            if(!isMyIdentifier(identifier)) { return; }
            ui->lb_Status->setText({});
            setWeatherGrid(weatherGrid);
        }

        void CWeatherComponent::setupConnections()
        {
            // UI connections
            connect(ui->cb_weatherScenario, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                    this, &CWeatherComponent::setWeatherScenario);
            connect(ui->le_LatOrIcao, &QLineEdit::returnPressed, this, &CWeatherComponent::updateWeatherInformation);
            connect(ui->le_Lon, &QLineEdit::returnPressed, this, &CWeatherComponent::updateWeatherInformation);
            connect(ui->cb_UseOwnAcftPosition, &QCheckBox::toggled, this, &CWeatherComponent::toggleUseOwnAircraftPosition);
            connect(&m_weatherUpdateTimer, &QTimer::timeout, this, &CWeatherComponent::updateWeatherInformation);

            // Context connections
            Q_ASSERT(sGui->getIContextSimulator());
            connect(sGui->getIContextSimulator(), &IContextSimulator::weatherGridReceived, this, &CWeatherComponent::weatherGridReceived);
        }

        void CWeatherComponent::setupInputValidators()
        {
            QRegExp reIcaoOrLatitude("^[a-zA-Z]{4}|-?\\d{1,2}[,.]?\\d+$", Qt::CaseInsensitive);
            ui->le_LatOrIcao->setValidator(new QRegExpValidator(reIcaoOrLatitude, this));
            QRegExp reLongitude("^-?\\d{1,2}[,.]?\\d+$", Qt::CaseInsensitive);
            ui->le_Lon->setValidator(new QRegExpValidator(reLongitude, this));
        }

        void CWeatherComponent::setupCompleter()
        {
            // Temporary list of ICAO airports. Replace with final list, once available
            QStringList airports = { "EDDM", "LSZH", "EDMO", "EGLL" };
            QCompleter *c = new QCompleter(airports, this);
            c->setCaseSensitivity(Qt::CaseInsensitive);
            c->setCompletionMode(QCompleter::PopupCompletion);
            c->setMaxVisibleItems(5);
            ui->le_LatOrIcao->setCompleter(c);
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
            ui->lb_Status->setText("Loading...");
            CWeatherGrid weatherGrid { { {}, position } };
            auto ident = identifier();
            sGui->getIContextSimulator()->requestWeatherGrid(weatherGrid, ident);
        }

    } // namespace
} // namespace
