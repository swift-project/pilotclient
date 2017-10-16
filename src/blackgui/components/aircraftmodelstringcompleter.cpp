/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelstringcompleter.h"
#include "ui_aircraftmodelstringcompleter.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackcore/webdataservices.h"
#include "blackcore/context/contextsimulator.h"
#include <QRadioButton>
#include <QStringListModel>

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Data;

namespace BlackGui
{
    namespace Components
    {
        CAircraftModelStringCompleter::CAircraftModelStringCompleter(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CAircraftModelStringCompleter)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "missing sGui");
            Q_ASSERT_X(sGui->hasWebDataServices(), Q_FUNC_INFO, "missing web services");

            ui->setupUi(this);
            ui->le_modelString->setValidator(new CUpperCaseValidator(ui->le_modelString));

            connect(ui->le_modelString, &QLineEdit::editingFinished, this, &CAircraftModelStringCompleter::onTextChanged);
            connect(sGui->getWebDataServices(), &CWebDataServices::swiftDbModelsRead, this, &CAircraftModelStringCompleter::onSwiftModelDataRead);
            connect(ui->rb_Db, &QRadioButton::clicked, this, &CAircraftModelStringCompleter::initGui);
            connect(ui->rb_ModelSet, &QRadioButton::clicked, this, &CAircraftModelStringCompleter::initGui);
            connect(ui->rb_OwnModels, &QRadioButton::clicked, this, &CAircraftModelStringCompleter::initGui);

            if (sGui->getIContextSimulator())
            {
                connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CAircraftModelStringCompleter::onSimulatorConnected);
                const CSimulatorInfo sim(sGui->getIContextSimulator()->getSimulatorPluginInfo().getSimulator());
                if (sim.isSingleSimulator())
                {
                    m_modelCaches.setCurrentSimulator(sim);
                }
                else
                {
                    this->setSourceVisible(OwnModels, false);
                }
            }
        }

        CAircraftModelStringCompleter::~CAircraftModelStringCompleter()
        { }

        QString CAircraftModelStringCompleter::getModelString() const
        {
            return ui->le_modelString->text();
        }

        void CAircraftModelStringCompleter::showSourceSelection(bool show)
        {
            ui->wi_SourceSelection->setVisible(!show);
        }

        void CAircraftModelStringCompleter::setText(const QString &completersString)
        {
            if (ui->le_modelString->text() == completersString) { return; }
            ui->le_modelString->setText(completersString);
        }

        void CAircraftModelStringCompleter::setModel(const BlackMisc::Simulation::CAircraftModel &model)
        {
            this->setText(model.getModelString());
        }

        void CAircraftModelStringCompleter::setSourceVisible(CompleterSource source, bool visible)
        {
            if (source.testFlag(DB)) { ui->rb_Db->setVisible(visible); }
            if (source.testFlag(ModelSet)) { ui->rb_ModelSet->setVisible(visible); }
            if (source.testFlag(OwnModels)) { ui->rb_OwnModels->setVisible(visible); }
        }

        void CAircraftModelStringCompleter::selectSource(CAircraftModelStringCompleter::CompleterSourceFlag source)
        {
            switch (source)
            {
            case DB: ui->rb_Db->setChecked(true); break;
            case ModelSet: ui->rb_ModelSet->setChecked(true); break;
            case OwnModels: ui->rb_OwnModels->setChecked(true); break;
            default: ui->rb_ModelSet->setChecked(true); break;
            }
        }

        void CAircraftModelStringCompleter::clear()
        {
            ui->le_modelString->clear();
        }

        void CAircraftModelStringCompleter::setCompleter()
        {
            QStringList modelStrings;
            CompleterSourceFlag sourceWithData = None;
            if (ui->rb_Db->isChecked())
            {
                if (m_currentSourceWithData == DB) { return; }
                modelStrings = sGui->getWebDataServices()->getModelCompleterStrings();
                if (!modelStrings.isEmpty()) { sourceWithData = DB; }
            }
            else if (ui->rb_ModelSet->isChecked())
            {
                if (m_currentSourceWithData == ModelSet) { return; }
                modelStrings = sGui->getIContextSimulator()->getModelSetCompleterStrings(true);
                if (!modelStrings.isEmpty()) { sourceWithData = ModelSet; }
            }
            else if (ui->rb_OwnModels->isChecked())
            {
                if (m_currentSourceWithData == OwnModels) { return; }
                modelStrings = m_modelCaches.getCurrentCachedModels().toCompleterStrings();
                if (!modelStrings.isEmpty()) { sourceWithData = OwnModels; }
            }

            m_currentSourceWithData = sourceWithData;
            ui->le_modelString->setCompleter(new QCompleter(modelStrings, this));
            ui->le_modelString->setPlaceholderText(QString("model strings (%1)").arg(modelStrings.size()));
        }

        void CAircraftModelStringCompleter::onTextChanged()
        {
            emit this->modelStringChanged();
        }

        void CAircraftModelStringCompleter::initGui()
        {
            this->setCompleter();
        }

        void CAircraftModelStringCompleter::onSimulatorConnected(int status)
        {
            // reinit because sim changed
            Q_UNUSED(status);
            this->initGui();
        }

        void CAircraftModelStringCompleter::onSwiftModelDataRead()
        {
            this->initGui();
        }
    } // ns
} // ns
