// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/dbloaddatadialog.h"
#include "ui_dbloaddatadialog.h"
#include "blackgui/guiapplication.h"
#include "core/webdataservices.h"
#include "core/db/databaseutils.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/logmessage.h"

#include <QDialogButtonBox>
#include <QModelIndexList>
#include <QPointer>
#include <QStringBuilder>

using namespace swift::misc;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::data;
using namespace swift::core;
using namespace swift::core::db;

namespace BlackGui::Components
{
    CDbLoadDataDialog::CDbLoadDataDialog(QWidget *parent) : QDialog(parent),
                                                            ui(new Ui::CDbLoadDataDialog)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        QStringListModel *lvm = new QStringListModel(ui->lv_Entities);
        ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
        ui->lv_Entities->setModel(lvm);
        ui->bb_loadDataDialog->button(QDialogButtonBox::Apply)->setText("Load");
        ui->wi_WorkStatus->setVisible(false);
        ui->wi_Consolidate->setVisible(false);
        ui->comp_SimulatorSelector->setRememberSelection(true);
        connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbLoadDataDialog::onDataRead, Qt::QueuedConnection);
        connect(sGui->getWebDataServices(), &CWebDataServices::entityDownloadProgress, this, &CDbLoadDataDialog::onEntityDownloadProgress, Qt::QueuedConnection);
        connect(ui->bb_loadDataDialog, &QDialogButtonBox::clicked, this, &CDbLoadDataDialog::onButtonClicked);
        connect(ui->pb_Consolidate, &QPushButton::clicked, this, &CDbLoadDataDialog::consolidate);
        connect(this, &CDbLoadDataDialog::rejected, this, &CDbLoadDataDialog::onRejected);
    }

    CDbLoadDataDialog::~CDbLoadDataDialog()
    {}

    bool CDbLoadDataDialog::newerOrEmptyEntitiesDetected(CEntityFlags::Entity loadEntities)
    {
        this->show();
        CGuiApplication::modalWindowToFront();
        if (m_consolidating) { return false; }
        if (m_pendingEntities != CEntityFlags::NoEntity) { return false; } // already loading
        if (loadEntities == CEntityFlags::NoEntity) { return false; }
        m_autoConsolidate = false;
        const QStringList entitiesStringList = CEntityFlags::entitiesToStringList(loadEntities);
        this->entitiesModel()->setStringList(entitiesStringList);
        ui->lv_Entities->selectAll();
        return true;
    }

    QStringListModel *CDbLoadDataDialog::entitiesModel() const
    {
        return qobject_cast<QStringListModel *>(ui->lv_Entities->model());
    }

    QStringList CDbLoadDataDialog::selectedEntities() const
    {
        QStringList entities;
        const QModelIndexList indexes = ui->lv_Entities->selectionModel()->selectedIndexes();
        for (const QModelIndex &index : indexes)
        {
            entities.append(index.data(Qt::DisplayRole).toString());
        }
        return entities;
    }

    void CDbLoadDataDialog::onButtonClicked(QAbstractButton *button)
    {
        if (!button) { return; }
        if (button == ui->bb_loadDataDialog->button(QDialogButtonBox::Apply))
        {
            const QStringList entityList = this->selectedEntities();
            if (entityList.isEmpty()) { return; }
            const CEntityFlags::Entity loadEntities = CEntityFlags::multipleEntitiesByNames(entityList);
            m_pendingEntities = sGui->getWebDataServices()->triggerLoadingDirectlyFromSharedFiles(loadEntities, false);
            const int pending = CEntityFlags::numberOfEntities(m_pendingEntities);
            m_pendingEntitiesCount = sGui->getWebDataServices()->getDbInfoObjectsCount(loadEntities);
            ui->pb_Loading->setMaximum(m_pendingEntitiesCount > 0 ? m_pendingEntitiesCount : pending);
            ui->pb_Loading->setValue(0);
            ui->wi_WorkStatus->setVisible(pending > 0);
            ui->wi_Consolidate->setVisible(false);
            ui->le_Info->setText("Loading started ...");
        }
    }

    void CDbLoadDataDialog::onDataRead(CEntityFlags::Entity entity, CEntityFlags::ReadState state, int number, const QUrl &url)
    {
        if (m_pendingEntities == CEntityFlags::NoEntity) { return; } // not triggered from here
        if (!m_pendingEntities.testFlag(CEntityFlags::entityToEntityFlag(entity))) { return; }

        const QString e = CEntityFlags::entitiesToString(entity);
        const QString s = CEntityFlags::stateToString(state);

        ui->le_Info->setText(e % u" " % s);
        if (!CEntityFlags::isFinishedReadStateOrFailure(state)) { return; }
        if (state == CEntityFlags::ReadFailed)
        {
            CLogMessage(this).warning(u"Read failed for %1 from '%2'") << e << url.toString();
        }

        m_pendingEntities &= ~entity;
        const int pending = CEntityFlags::numberOfEntities(m_pendingEntities);
        const int max = ui->pb_Loading->maximum();
        if (m_pendingEntitiesCount < 0)
        {
            ui->pb_Loading->setValue(max - pending);
        }
        else
        {
            m_pendingEntitiesCount -= number;
            ui->pb_Loading->setValue(max - m_pendingEntitiesCount);
        }
        if (pending < 1)
        {
            m_pendingEntitiesCount = -1;
            const bool defaultConsolidate = !ui->cb_AllModels->isChecked() && ui->cb_ModelSet->isChecked();

            QPointer<CDbLoadDataDialog> myself(this);
            QTimer::singleShot(2000, this, [=] {
                if (!myself) { return; }
                ui->wi_Consolidate->setVisible(true);
                ui->wi_WorkStatus->setVisible(false);
                if (defaultConsolidate)
                {
                    m_autoConsolidate = true;
                    QPointer<CDbLoadDataDialog> self(this); // 2nd "self"/"myself" for cppcheck identicalConditionAfterEarlyExit
                    QTimer::singleShot(1000, this, [=] {
                        if (!self) { return; }
                        self->consolidate();
                    });
                }
            });
        }
    }

    void CDbLoadDataDialog::onEntityDownloadProgress(CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url)
    {
        Q_UNUSED(entity)
        Q_UNUSED(logId)
        Q_UNUSED(progress)
        Q_UNUSED(current)
        Q_UNUSED(max)
        Q_UNUSED(url)
    }

    void CDbLoadDataDialog::onRejected()
    {
        m_pendingEntities = CEntityFlags::NoEntity;
        m_pendingEntitiesCount = -1;
        m_autoConsolidate = false;
        ui->pb_Loading->setVisible(false);
    }

    void CDbLoadDataDialog::consolidate()
    {
        const bool set = ui->cb_ModelSet->isChecked();
        const bool all = ui->cb_AllModels->isChecked();
        if (m_consolidating) { return; }
        if (!set && !all) { return; }
        ui->wi_WorkStatus->setVisible(true);
        ui->pb_Loading->setValue(0);
        ui->pb_Loading->setMaximum(0); // 0/0 causing busy indicator
        const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();

        do
        {
            if (set)
            {
                ui->le_Info->setText("Model set");
                CAircraftModelList models = m_sets.getCachedModels(simulator);
                const int c = CDatabaseUtils::consolidateModelsWithDbDataAllowsGuiRefresh(models, true, true);
                if (c > 0) { m_sets.setCachedModels(models, simulator); }
            }

            if (!this->isVisible()) { break; } // dialog closed?
            if (all)
            {
                ui->le_Info->setText("All models");
                CAircraftModelList models = m_models.getCachedModels(simulator);
                const int c = CDatabaseUtils::consolidateModelsWithDbDataAllowsGuiRefresh(models, true, true);
                if (c > 0) { m_models.setCachedModels(models, simulator); }
            }
        }
        while (false);

        m_consolidating = false;

        QPointer<CDbLoadDataDialog> myself(this);
        QTimer::singleShot(2000, this, [=] {
            if (!myself) { return; }
            ui->pb_Loading->setMaximum(100);
            ui->wi_WorkStatus->setVisible(false);
            if (m_autoConsolidate)
            {
                m_autoConsolidate = false;
                this->accept();
            }
        });
    }
} // ns
