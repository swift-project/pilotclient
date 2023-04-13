/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "aircraftmodelstringcompleter.h"
#include "ui_aircraftmodelstringcompleter.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackcore/webdataservices.h"
#include "blackcore/context/contextsimulator.h"

#include <QRadioButton>
#include <QStringListModel>
#include <QPointer>

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Data;

namespace BlackGui::Components
{
    CAircraftModelStringCompleter::CAircraftModelStringCompleter(QWidget *parent) : QFrame(parent),
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
        connect(&m_modelCaches, &CModelCaches::cacheChanged, this, &CAircraftModelStringCompleter::setSimulator, Qt::QueuedConnection);

        CSimulatorInfo simulator = CSimulatorInfo(CSimulatorInfo::P3D); // default
        if (sGui->getIContextSimulator())
        {
            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CAircraftModelStringCompleter::onSimulatorConnected, Qt::QueuedConnection);
            CSimulatorInfo pluginSimulator = sGui->getIContextSimulator()->getSimulatorPluginInfo().getSimulator();
            if (pluginSimulator.isSingleSimulator())
            {
                simulator = pluginSimulator;
            }
            else
            {
                this->setSourceVisible(OwnModels, false); // hide own models
            }
        }
        this->setSimulator(simulator);
        this->initGui();
    }

    CAircraftModelStringCompleter::~CAircraftModelStringCompleter()
    {}

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

    bool CAircraftModelStringCompleter::setSimulator(const CSimulatorInfo &simulator)
    {
        if (simulator.isSingleSimulator()) { return false; }
        if (this->getSimulator() == simulator) { return false; }
        m_currentSimulator = simulator;
        QPointer<CAircraftModelStringCompleter> myself(this);
        QTimer::singleShot(100, this, [=] {
            if (!myself) { return; }
            this->setCompleter(true);
        });
        return true;
    }

    CSimulatorInfo CAircraftModelStringCompleter::getSimulator() const
    {
        return m_currentSimulator;
    }

    void CAircraftModelStringCompleter::clear()
    {
        ui->le_modelString->clear();
    }

    void CAircraftModelStringCompleter::setCompleter(bool simChanged)
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        QStringList modelStrings;
        CompleterSourceFlag dataSource = None;
        QString simInfo = m_currentSimulator.toQString();
        if (ui->rb_Db->isChecked())
        {
            if (!simChanged && m_currentDataSource == DB) { return; }
            modelStrings = sGui->getWebDataServices()->getModelCompleterStrings();
            dataSource = DB;
            simInfo = QStringLiteral("DB models");
        }
        else if (ui->rb_ModelSet->isChecked() && sGui && sGui->getIContextSimulator())
        {
            if (!simChanged && m_currentDataSource == ModelSet) { return; }
            modelStrings = sGui->getIContextSimulator()->getModelSetCompleterStrings(true);
            dataSource = ModelSet;
        }
        else if (ui->rb_OwnModels->isChecked())
        {
            if (!simChanged && m_currentDataSource == OwnModels) { return; }
            modelStrings = m_modelCaches.getCachedModels(this->getSimulator()).toCompleterStrings();
            dataSource = OwnModels;
        }

        m_currentDataSource = dataSource;
        ui->le_modelString->setCompleter(new QCompleter(modelStrings, this));
        ui->le_modelString->setPlaceholderText(QStringLiteral("model strings (%1/%2)").arg(modelStrings.size()).arg(simInfo));
    }

    void CAircraftModelStringCompleter::onTextChanged()
    {
        emit this->modelStringChanged();
    }

    void CAircraftModelStringCompleter::initGui()
    {
        this->setCompleter(true);
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
