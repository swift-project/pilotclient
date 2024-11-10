// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/db/databaseutils.h"
#include "dbautosimulatorstashingcomponent.h"
#include "gui/components/dbmappingcomponent.h"
#include "ui_dbautosimulatorstashingcomponent.h"
#include <QIntValidator>

using namespace swift::gui;
using namespace swift::core::db;
using namespace swift::misc;
using namespace swift::misc::simulation;

namespace swift::gui::components
{
    CDbAutoSimulatorStashingComponent::CDbAutoSimulatorStashingComponent(QWidget *parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
                                                                                            CDbMappingComponentAware(qobject_cast<CDbMappingComponent *>(parent)),
                                                                                            ui(new Ui::CDbAutoSimulatorStashingComponent)
    {
        ui->setupUi(this);
        ui->le_MaxModelsStashed->setValidator(new QIntValidator(this));
        ui->tvp_StatusMessages->setMode(swift::gui::models::CStatusMessageListModel::Simplified);
        ui->le_MaxModelsStashed->setText("100");
    }

    int CDbAutoSimulatorStashingComponent::exec()
    {
        this->initGui();
        return QDialog::exec();
    }

    void CDbAutoSimulatorStashingComponent::accept()
    {
        switch (m_state)
        {
        case Running: return;
        case Completed:
        {
            if (!m_modelsToStash.isEmpty())
            {
                // this removes previously stashed models
                this->getMappingComponent()->replaceStashedModelsUnvalidated(m_modelsToStash);
                const CStatusMessage stashedMsg(this, CStatusMessage::SeverityInfo, QStringLiteral("Stashed %1 models").arg(m_modelsToStash.size()));
                this->addStatusMessage(stashedMsg);
                m_modelsToStash.clear();
            }
            QDialog::accept();
            break;
        }
        default:
        {
            this->tryToStash();
            break;
        }
        }
    }

    CDbAutoSimulatorStashingComponent::~CDbAutoSimulatorStashingComponent()
    {}

    void CDbAutoSimulatorStashingComponent::updateProgressIndicator(int percent)
    {
        if (percent > 100) { percent = 100; }
        if (percent < 0) { percent = 0; }
        ui->pb_StashingProgress->setValue(percent);
    }

    views::CAircraftModelView *CDbAutoSimulatorStashingComponent::currentModelView() const
    {
        return this->getMappingComponent()->currentModelView();
    }

    void CDbAutoSimulatorStashingComponent::initGui()
    {
        ui->bb_OkCancel->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        ui->tvp_StatusMessages->clear();
        m_state = Idle;
        this->updateProgressIndicator(0);

        const QString infoAll = this->getMappingComponent()->getOwnModelsInfoStringFsFamily();
        ui->le_AllSets->setText(infoAll);

        if (!this->currentModelView())
        {
            const CStatusMessage m(this, CStatusMessage::SeverityError, u"No data for simulator updating");
            this->addStatusMessage(m);
        }
        else
        {
            const int selected = this->currentModelView()->selectedRowCount();
            ui->le_Selected->setText(QString::number(selected));
        }
    }

    void CDbAutoSimulatorStashingComponent::addStatusMessage(const CStatusMessage &msg)
    {
        if (msg.isEmpty()) { return; }
        ui->tvp_StatusMessages->insert(msg);
    }

    void CDbAutoSimulatorStashingComponent::addStatusMessages(const CStatusMessageList &msgs)
    {
        if (msgs.isEmpty()) { return; }
        for (const CStatusMessage &msg : msgs)
        {
            this->addStatusMessage(msg);
        }
    }

    void CDbAutoSimulatorStashingComponent::addStatusMessage(const CStatusMessage &msg, const CAircraftModel &model)
    {
        if (msg.isEmpty()) { return; }
        if (model.hasModelString())
        {
            CStatusMessage prefixMessage(msg);
            prefixMessage.prependMessage(QString(model.getModelString() + ", " + model.getMembersDbStatus() + ": "));
            ui->tvp_StatusMessages->insert(prefixMessage);
        }
        else
        {
            ui->tvp_StatusMessages->insert(msg);
        }
    }

    void CDbAutoSimulatorStashingComponent::tryToStash()
    {
        Q_ASSERT_X(this->getMappingComponent(), Q_FUNC_INFO, "Missing mapping component");

        if (!this->currentModelView()) { return; }
        m_state = Running;
        int maxObjectsStashed = -1;
        if (!ui->le_MaxModelsStashed->text().isEmpty())
        {
            bool ok;
            ui->le_MaxModelsStashed->text().toInt(&ok);
            if (!ok) { maxObjectsStashed = 100; }
        }

        const bool selected = ui->rb_SelectedOnly->isChecked();
        int ownModelsCount = 0;
        CStatusMessageList info;
        if (selected)
        {
            static const QString intro("Checking %1 selected models");
            const CAircraftModelList selectedModels(this->currentModelView()->selectedObjects());
            ownModelsCount = selectedModels.size();
            this->addStatusMessage(CStatusMessage(this, CStatusMessage::SeverityInfo, intro.arg(ownModelsCount)));
            m_modelsToStash = CDatabaseUtils::updateSimulatorForFsFamily(selectedModels, &info, maxObjectsStashed, this, true);
        }
        else
        {
            const CDbMappingComponent *mappincComponent = this->getMappingComponent();
            const QSet<CSimulatorInfo> fsFamilySims(CSimulatorInfo::allFsFamilySimulators().asSingleSimulatorSet());
            static const QString intro("Checking %1 models for %2");

            // check all own models
            for (const CSimulatorInfo &simulator : fsFamilySims)
            {
                const CAircraftModelList ownModels = mappincComponent->getOwnCachedModels(simulator);
                const QString sim = simulator.toQString();
                ownModelsCount += ownModels.size();
                this->addStatusMessage(CStatusMessage(this, CStatusMessage::SeverityInfo, intro.arg(ownModels.size()).arg(sim)));
                m_modelsToStash.push_back(CDatabaseUtils::updateSimulatorForFsFamily(ownModels, &info, maxObjectsStashed, this, true));
            }
        }

        const QString result("Tested %1 own models, %2 models should be updated in DB");
        this->addStatusMessages(info);
        this->addStatusMessage(CStatusMessage(this, CStatusMessage::SeverityInfo, result.arg(ownModelsCount).arg(m_modelsToStash.size())));
        m_state = Completed;
    }
} // ns
