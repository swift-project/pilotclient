/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbownmodelsetcomponent.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackgui/models/aircrafticaolistmodel.h"
#include "dbmappingcomponent.h"
#include "dbownmodelsetdialog.h"
#include "ui_dbownmodelsetcomponent.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Models;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbOwnModelSetComponent::CDbOwnModelSetComponent(QWidget *parent) :
            QFrame(parent),
            CDbMappingComponentAware(parent),
            ui(new Ui::CDbOwnModelSetComponent)
        {
            ui->setupUi(this);
            ui->tvp_OwnModelSet->setAircraftModelMode(CAircraftModelListModel::OwnSimulatorModelMapping);
            ui->tvp_OwnModelSet->menuRemoveItems(CAircraftModelView::MenuDisplayAutomaticallyAndRefresh | CAircraftModelView::MenuStashing | CAircraftModelView::MenuBackend | CAircraftModelView::MenuRefresh);
            ui->tvp_OwnModelSet->menuAddItems(CAircraftModelView::MenuRemoveSelectedRows | CAircraftModelView::MenuClear);
            ui->tvp_OwnModelSet->addFilterDialog();
            connect(ui->pb_CreateNewSet, &QPushButton::clicked, this, &CDbOwnModelSetComponent::ps_buttonClicked);
            connect(ui->pb_LoadExistingSet, &QPushButton::clicked, this, &CDbOwnModelSetComponent::ps_buttonClicked);
        }

        CDbOwnModelSetComponent::~CDbOwnModelSetComponent()
        {
            // void
        }

        Views::CAircraftModelView *CDbOwnModelSetComponent::view() const
        {
            return ui->tvp_OwnModelSet;
        }

        void CDbOwnModelSetComponent::setModelSet(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            this->ui->tvp_OwnModelSet->updateContainerMaybeAsync(models);
            this->ui->pb_SaveAsSetForSimulator->setText("save for " + simulator.toQString());
            this->ui->pb_SaveAsSetForSimulator->setEnabled(!models.isEmpty());
            this->m_simulator = simulator;
            this->setSaveFileName(simulator);
        }

        void CDbOwnModelSetComponent::setMappingComponent(CDbMappingComponent *component)
        {
            CDbMappingComponentAware::setMappingComponent(component);
            if (component)
            {
                connect(this->getMappingComponent(), &CDbMappingComponent::tabIndexChanged, this, &CDbOwnModelSetComponent::ps_tabChanged);
            }
        }

        void CDbOwnModelSetComponent::ps_tabChanged(int index)
        {
            CDbMappingComponent::TabIndex ti = static_cast<CDbMappingComponent::TabIndex>(index);
            if (ti == CDbMappingComponent::TabOwnModelSet)
            {
                // myself
                this->getMappingComponent()->resizeForSelect();
            }
            else
            {
                // others
            }
        }

        void CDbOwnModelSetComponent::ps_buttonClicked()
        {
            const QObject *sender = QObject::sender();
            if (sender == ui->pb_CreateNewSet)
            {
                if (!this->m_modelSetDialog)
                {
                    this->m_modelSetDialog.reset(new CDbOwnModelSetDialog(this));
                    this->m_modelSetDialog->setMappingComponent(this->getMappingComponent());
                }

                if (this->getMappingComponent()->getOwnModelsCount() > 0)
                {
                    this->m_modelSetDialog->setModal(true);
                    this->m_modelSetDialog->reloadData();
                    QDialog::DialogCode rc = static_cast<QDialog::DialogCode>(
                                                 this->m_modelSetDialog->exec()
                                             );
                    if (rc == QDialog::Accepted)
                    {
                        this->setModelSet(this->m_modelSetDialog->getModelSet(), this->m_modelSetDialog->getSimulatorInfo());
                    }
                }
                else
                {
                    static const CStatusMessage m(CStatusMessage(this, CStatusMessage::SeverityError, "No model data"));
                    this->getMappingComponent()->showOverlayMessage(m);
                }
            }
            else if (sender == ui->pb_LoadExistingSet)
            {
                this->ui->tvp_OwnModelSet->showFileLoadDialog();
            }
        }

        void CDbOwnModelSetComponent::setSaveFileName(const CSimulatorInfo &sim)
        {
            Q_ASSERT_X(sim.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            const QString n("modelset" + sim.toQString(true));
            this->ui->tvp_OwnModelSet->setSaveFileName(n);
        }
    } // ns
} // ns
