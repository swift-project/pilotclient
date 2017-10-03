/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/aircraftcombinedtypeselector.h"
#include "blackgui/components/dbaircrafticaoselectorcomponent.h"
#include "blackgui/components/dbairlineicaoselectorcomponent.h"
#include "blackgui/components/dbliveryselectorcomponent.h"
#include "blackgui/components/modelmatchercomponent.h"
#include "blackgui/components/simulatorselector.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/models/statusmessagelistmodel.h"
#include "blackgui/uppercasevalidator.h"
#include "blackgui/views/statusmessageview.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/network/user.h"
#include "blackmisc/statusmessagelist.h"
#include "ui_modelmatchercomponent.h"

#include <QCheckBox>
#include <QCompleter>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QTabWidget>
#include <QTextEdit>
#include <QtGlobal>

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
            ui->comp_AirlineSelector->displayWithIcaoDescription(false);
            ui->comp_AircraftSelector->displayWithIcaoDescription(false);
            ui->comp_LiverySelector->withLiveryDescription(false);
            ui->tvp_ResultMessages->setMode(CStatusMessageListModel::Simplified);

            const CUpperCaseValidator *validator = new CUpperCaseValidator(this);
            ui->le_ModelString->setValidator(validator);
            ui->le_Manufacturer->setValidator(validator);
            ui->le_Callsign->setValidator(validator);

            connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &CModelMatcherComponent::ps_simulatorChanged);
            connect(ui->pb_ModelMatching, &QPushButton::pressed, this, &CModelMatcherComponent::ps_testModelMatching);
            connect(ui->pb_ReverseLookup, &QPushButton::pressed, this, &CModelMatcherComponent::ps_reverseLookup);
            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CModelMatcherComponent::ps_webDataRed);

            const CSimulatorInfo sim(this->m_modelSetLoader.getSimulator());
            ui->comp_SimulatorSelector->setValue(sim);
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
            this->m_matcher.setModelSet(this->m_modelSetLoader.getAircraftModels(), simulator);
            this->redisplay();
        }

        void CModelMatcherComponent::ps_cacheChanged(CSimulatorInfo &simulator)
        {
            Q_UNUSED(simulator);
            this->redisplay();
        }

        void CModelMatcherComponent::ps_testModelMatching()
        {
            ui->te_Results->clear();
            CStatusMessageList msgs;
            this->ps_simulatorChanged(ui->comp_SimulatorSelector->getValue()); // update model set to latest version
            CSimulatedAircraft remoteAircraft(createAircraft());
            if (ui->cb_withReverseLookup->isChecked())
            {
                const QString liveryString(ui->comp_LiverySelector->getRawCombinedCode());
                const CAircraftModel reverseModel = CAircraftMatcher::reverseLookupModel(remoteAircraft.getModel(), liveryString, &msgs);
                remoteAircraft.setModel(reverseModel);
            }

            this->m_matcher.setDefaultModel(CModelMatcherComponent::defaultModel());
            const CAircraftModel matched = this->m_matcher.getClosestMatch(remoteAircraft, &msgs); // test model matching
            ui->te_Results->setText(matched.toQString(true));
            ui->tvp_ResultMessages->updateContainer(msgs);
        }

        void CModelMatcherComponent::ps_reverseLookup()
        {
            ui->te_Results->clear();
            CStatusMessageList msgs;
            this->m_matcher.setDefaultModel(CModelMatcherComponent::defaultModel());
            const CSimulatedAircraft remoteAircraft(createAircraft());
            const QString livery(ui->comp_LiverySelector->getRawCombinedCode());
            const CAircraftModel matched = CAircraftMatcher::reverseLookupModel(remoteAircraft.getModel(), livery, &msgs);
            ui->te_Results->setText(matched.toQString(true));
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
            const QString modelString(ui->le_ModelString->text().trimmed().toUpper());
            const QString combined(ui->comp_CombinedCode->getCombinedType());
            const QString manufacturer(ui->le_Manufacturer->text().trimmed().toUpper());
            const QString liveryCombinedCode(ui->comp_LiverySelector->getRawCombinedCode());
            const CCallsign cs(ui->le_Callsign->text().isEmpty() ? "SWIFT" : ui->le_Callsign->text()); // need callsign
            static const CUser pilot("123456", "swift Test", cs);

            CAircraftIcaoCode icao(aircraft, combined);
            icao.setManufacturer(manufacturer);

            const CAirlineIcaoCode airlineIcao(airline);
            const CLivery livery(liveryCombinedCode, airlineIcao, "");
            CAircraftModel m(modelString, CAircraftModel::TypeFSInnData);
            m.setLivery(livery);
            m.setCallsign(cs);
            m.setModelType(modelString.isEmpty() ? CAircraftModel::TypeQueriedFromNetwork : CAircraftModel::TypeFSInnData);
            CSimulatedAircraft sa(m);
            sa.setPilot(pilot);
            sa.setAircraftIcaoCode(icao);
            return sa;
        }

        CAircraftModel CModelMatcherComponent::defaultModel() const
        {
            // can somehow dynamilcally determine the models
            const CAircraftIcaoCode icaoAircraft("B737", "L2J", "FooBar", "Dummy", "M", false, false, false, 1);
            const CAirlineIcaoCode icaoAirline("Foo", "FooBar airlines", { "DE", "Germany" }, "FOO", true, true);
            const CLivery livery(CLivery::getStandardCode(icaoAirline), icaoAirline, "Standard Foo airlines", "red", "blue", false);
            CAircraftModel model("default model", CAircraftModel::TypeOwnSimulatorModel, "dummy model", icaoAircraft, livery);
            if (model.getCallsign().isEmpty()) { model.setCallsign("SWIFT"); }
            return model;
        }
    } // ns
} // ns
