/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "modelmatchercomponent.h"
#include "guiapplication.h"
#include "guiutility.h"
#include "uppercasevalidator.h"
#include "ui_modelmatchercomponent.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackGui::Models;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CModelMatcherComponent::CModelMatcherComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CModelMatcherComponent)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
            Q_ASSERT_X(sGui->getWebDataServices(), Q_FUNC_INFO, "Missing web services");

            ui->setupUi(this);
            ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
            ui->comp_AirlineSelector->withIcaoDescription(false);
            ui->comp_AircraftSelector->withIcaoDescription(false);
            ui->comp_LiverySelector->withLiveryDescription(false);
            ui->tvp_ResultMessages->setMode(CStatusMessageListModel::Simplified);

            const CUpperCaseValidator *validator = new CUpperCaseValidator(this);
            ui->le_ModelString->setValidator(validator);
            ui->le_Manufacturer->setValidator(validator);

            connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &CModelMatcherComponent::ps_simulatorChanged);
            connect(ui->pb_Test, &QPushButton::pressed, this, &CModelMatcherComponent::ps_test);
            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CModelMatcherComponent::ps_webDataRed);

            const CSimulatorInfo sim(ui->comp_SimulatorSelector->getValue());
            this->m_modelSetLoader.changeSimulator(sim);
            this->redisplay();
        }

        CModelMatcherComponent::~CModelMatcherComponent()
        { }

        void CModelMatcherComponent::tabIndexChanged(int index)
        {
            if (index < 0) { return; }
            QTabWidget *tw = CGuiUtility::parentTabWidget(this);
            Q_ASSERT_X(tw, Q_FUNC_INFO, "Cannot find parent tab widget");
            const QWidget *tabWidget = tw->currentWidget();
            const QWidget *myselfTabWidget = this->parentWidget();
            if (tabWidget != myselfTabWidget) { return; }
            this->redisplay();
        }

        void CModelMatcherComponent::ps_simulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            this->m_modelSetLoader.changeSimulator(simulator);
            this->m_matcher.setModelSet(this->m_modelSetLoader.getAircraftModels());
            this->redisplay();
        }

        void CModelMatcherComponent::ps_cacheChanged(CSimulatorInfo &simulator)
        {
            Q_UNUSED(simulator);
            this->redisplay();
        }

        void CModelMatcherComponent::ps_test()
        {
            ui->te_Results->clear();
            CStatusMessageList msgs;
            this->m_matcher.setDefaultModel(CModelMatcherComponent::defaultModel());
            CSimulatedAircraft remoteAircraft(createAircraft());
            const CAircraftModel matched = this->m_matcher.getClosestMatch(remoteAircraft, &msgs);
            remoteAircraft.setModel(matched);
            ui->te_Results->setText(remoteAircraft.toQString(true));
            ui->tvp_ResultMessages->updateContainer(msgs);
        }

        void CModelMatcherComponent::ps_webDataRed(CEntityFlags::Entity entity, CEntityFlags::ReadState state, int number)
        {
            if (number > 0 && entity.testFlag(CEntityFlags::ModelEntity) && state == CEntityFlags::ReadFinished)
            {
                QStringList modelStrings(sGui->getWebDataServices()->getModelStrings());
                modelStrings.sort();
                ui->le_ModelString->setCompleter(new QCompleter(modelStrings, this));
            }
        }

        void CModelMatcherComponent::redisplay()
        {
            const int c = this->m_modelSetLoader.getAircraftModelsCount();
            ui->le_ModelSetCount->setText(QString::number(c));
        }

        CSimulatedAircraft CModelMatcherComponent::createAircraft() const
        {
            const QString airline(ui->comp_AirlineSelector->getRawDesignator());
            const QString aircraft(ui->comp_AircraftSelector->getRawDesignator());
            const QString model(ui->le_ModelString->text().trimmed().toUpper());
            const QString combined(ui->comp_CombinedCode->getCombinedType());
            const QString manufacturer(ui->le_Manufacturer->text().trimmed().toUpper());

            CCallsign cs("SWIFT");
            CSimulatedAircraft sa;
            if (!model.isEmpty())
            {
                const CAircraftModel m(model, CAircraftModel::TypeFsdData);
                sa = CSimulatedAircraft(m);
            }
            sa.setCallsign(cs);

            if (!aircraft.isEmpty())
            {
                CAircraftIcaoCode icao(aircraft, combined);
                icao.setManufacturer(manufacturer);
                sa.setAircraftIcaoCode(icao);
            }
            if (!airline.isEmpty())
            {
                const CAirlineIcaoCode al(airline);
                const CLivery l(CLivery::getStandardCode(al), al, "Standard");
                sa.setLivery(l);
            }
            return sa;
        }

        CAircraftModel CModelMatcherComponent::defaultModel() const
        {
            // can somehow dynamilcally determine the models
            const CAircraftIcaoCode icaoAircraft("B737", "L2J", "FooBar", "Dummy", "M", false, false, false, 1);
            const CAirlineIcaoCode icaoAirline("Foo", "FooBar airlines", { "DE", "Germany" }, "FOO", true, true);
            const CLivery livery(CLivery::getStandardCode(icaoAirline), icaoAirline, "Standard Foo airlines", "red", "blue", false);
            const CAircraftModel model("default model", CAircraftModel::TypeOwnSimulatorModel, "dummy model", icaoAircraft, livery);
            return model;
        }
    } // ns
} // ns
