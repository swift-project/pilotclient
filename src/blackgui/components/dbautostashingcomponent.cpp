/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ui_dbautostashingcomponent.h"
#include "dbautostashingcomponent.h"
#include "dbmappingcomponent.h"
#include "dbstashcomponent.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include <QIntValidator>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbAutoStashingComponent::CDbAutoStashingComponent(QWidget *parent) :
            QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
            CDbMappingComponentAware(qobject_cast<CDbMappingComponent * >(parent)),
            ui(new Ui::CDbAutoStashingComponent)
        {
            ui->setupUi(this);
            this->ui->tvp_StatusMessages->setResizeMode(CAircraftModelView::ResizingAuto);
            this->ui->tvp_StatusMessages->menuAddItems(CAircraftModelView::MenuSave);
            this->ui->le_MaxModelsStashed->setValidator(new QIntValidator(10, CDbStashComponent::MaxModelPublished, this));
            Q_ASSERT_X(this->getMappingComponent(), Q_FUNC_INFO, "Expect mapping componet");
        }

        CDbAutoStashingComponent::~CDbAutoStashingComponent()
        { }

        void CDbAutoStashingComponent::setProvider(IWebDataServicesProvider *webDataReaderProvider)
        {
            CWebDataServicesAware::setProvider(webDataReaderProvider);
            connectDataReadSignal(
                this,
                std::bind(&CDbAutoStashingComponent::ps_entitiesRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
            );
        }

        void CDbAutoStashingComponent::accept()
        {
            if (m_state == Running) { return; }
            if (m_state == Completed)
            {
                if (!this->m_modelsToStash.isEmpty())
                {
                    // this removes previously stashed models
                    this->getMappingComponent()->replaceStashedModelsUnvalidated(this->m_modelsToStash);
                    if (this->ui->cb_RemovedChecked->isChecked())
                    {
                        this->currentModelView()->removeModelsWithModelString(this->m_modelsToStash);
                    }
                    const CStatusMessage stashedMsg(categgories(), CStatusMessage::SeverityInfo, QString("Auto stashed %1 models").arg(m_modelsToStash.size()));
                    this->addStatusMessage(stashedMsg);
                    this->m_modelsToStash.clear();
                }
                QDialog::accept();
            }
            if (this->getSelectedOrAllCount() < 1)
            {
                const CStatusMessage m(categgories(), CStatusMessage::SeverityError, "No data, nothing to do");
                this->addStatusMessage(m);
                QDialog::accept();
            }
            this->tryToStashModels();
        }

        int CDbAutoStashingComponent::exec()
        {
            this->initGui();
            return QDialog::exec();
        }

        void CDbAutoStashingComponent::showLastResults()
        {
            this->ui->bb_AutoStashing->setStandardButtons(QDialogButtonBox::Close);
            this->setVisible(true);
        }

        void CDbAutoStashingComponent::ps_entitiesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            if (!readState != CEntityFlags::ReadFinished) { return; }
            Q_UNUSED(count);
            Q_UNUSED(entity);
        }

        void CDbAutoStashingComponent::initGui()
        {
            this->m_state = Idle;
            this->ui->bb_AutoStashing->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
            this->ui->tvp_StatusMessages->clear();
            this->m_noData = 0;
            this->m_noValidationFailed = 0;
            this->m_noStashed = 0;
            this->updateGuiValues(0);

            if (!this->currentModelView())
            {
                const CStatusMessage m(categgories(), CStatusMessage::SeverityError, "No data for auto stashing");
                this->addStatusMessage(m);
            }
            else
            {
                int selected = this->currentModelView()->selectedRowCount();
                int all = this->currentModelView()->rowCount();
                this->ui->le_Selected->setText(QString::number(selected));
                QString allStr(QString::number(all));
                if (all > CDbStashComponent::MaxModelPublished)
                {
                    allStr += " (Max." + QString::number(CDbStashComponent::MaxModelPublished) + ")";
                }
                this->ui->le_All->setText(allStr);
                if (this->ui->le_MaxModelsStashed->text().isEmpty())
                {
                    this->ui->le_MaxModelsStashed->setText(all > 100 ? "100" : "");
                }
                if (selected > 0)
                {
                    this->ui->rb_Selected->setChecked(true);
                    this->ui->rb_Selected->setEnabled(true);
                }
                else
                {
                    this->ui->rb_All->setChecked(true);
                    this->ui->rb_Selected->setEnabled(false);
                }
            }
        }

        void CDbAutoStashingComponent::updateGuiValues(int percent)
        {
            if (percent > 100) { percent = 100; }
            if (percent < 0) { percent = 0; }
            this->ui->pb_StashingProgress->setValue(percent);
            this->ui->le_Stashed->setText(QString::number(this->m_noStashed));
            this->ui->le_NoData->setText(QString::number(this->m_noData));
            this->ui->le_ValidationFailed->setText(QString::number(this->m_noValidationFailed));
        }

        int CDbAutoStashingComponent::getSelectedOrAllCount() const
        {
            if (!this->currentModelView()) { return 0; }
            if (this->ui->rb_Selected->isChecked())
            {
                return this->currentModelView()->selectedRowCount();
            }
            else
            {
                return this->currentModelView()->rowCount();
            }
        }

        const CAircraftModelView *CDbAutoStashingComponent::currentModelView() const
        {
            return this->getMappingComponent()->currentModelView();
        }

        CAircraftModelView *CDbAutoStashingComponent::currentModelView()
        {
            return this->getMappingComponent()->currentModelView();
        }

        void CDbAutoStashingComponent::addStatusMessage(const CStatusMessage &msg)
        {
            if (msg.isEmpty()) { return; }
            this->ui->tvp_StatusMessages->insert(msg);
        }

        void CDbAutoStashingComponent::addStatusMessage(const CStatusMessage &msg, const CAircraftModel &model)
        {
            if (msg.isEmpty()) { return; }
            if (model.hasModelString())
            {
                CStatusMessage prefixMessage(msg);
                prefixMessage.prependMessage(QString(model.getModelString() + ", " + model.getMembersDbStatus() + ": "));
                this->ui->tvp_StatusMessages->insert(prefixMessage);
            }
            else
            {
                this->ui->tvp_StatusMessages->insert(msg);
            }
        }

        void CDbAutoStashingComponent::tryToStashModels()
        {
            Q_ASSERT_X(this->currentModelView(), Q_FUNC_INFO, "No view");
            const CAircraftModelList models(this->ui->rb_Selected->isChecked() ? this->currentModelView()->selectedObjects() : this->currentModelView()->containerOrFilteredContainer());
            if (models.isEmpty()) { return; }

            // we have data and are good to go
            this->m_state = Running;
            const int all = models.size();

            // maximum
            const QString maxStr(this->ui->le_MaxModelsStashed->text());
            bool okMaxStr = true;
            int max = maxStr.isEmpty() ? CDbStashComponent::MaxModelPublished : maxStr.toInt(&okMaxStr);
            if (!okMaxStr || max > all) { max = all; }

            // override description
            const QString description(this->ui->le_Description->text().trimmed());

            int c = 0;
            CAircraftModelList autoStashed;
            for (const CAircraftModel &model : models)
            {
                CAircraftModel stashModel(model);
                bool stashed = this->tryToStashModel(stashModel);
                if (stashed)
                {
                    if (!description.isEmpty()) { stashModel.setDescription(description); }
                    autoStashed.push_back(stashModel);
                }

                c++;
                if (c % 25 == 0)
                {
                    Q_ASSERT_X(c <= all, Q_FUNC_INFO, "illegal numbers");
                    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

                    int percent = c * 100 / all;
                    if (max < all)
                    {
                        int maxPercent = autoStashed.size() * 100 / max;
                        if (maxPercent > percent) { percent = maxPercent; }
                    }
                    this->updateGuiValues(percent);
                }
                if (autoStashed.size() >= max) { break; }
            }

            this->updateGuiValues(100);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

            const CStatusMessage stashedMsg(categgories(), CStatusMessage::SeverityInfo, QString("Ready to auto stashed %1 models").arg(autoStashed.size()));
            this->addStatusMessage(stashedMsg);
            this->m_modelsToStash = autoStashed;
            this->m_state = Completed;
        }

        bool CDbAutoStashingComponent::tryToStashModel(Simulation::CAircraftModel &model)
        {
            bool stashed = false;

            //! Some upfront tests
            if (!model.hasModelString())
            {
                this->addStatusMessage(CStatusMessage(this->categgories(), CStatusMessage::SeverityError, "No model string"));
                this->m_noData++;
            }
            else if (!model.hasAircraftDesignator())
            {
                this->addStatusMessage(CStatusMessage(this->categgories(), CStatusMessage::SeverityError, "No aircraft designator"), model);
                this->m_noData++;
            }
            else if (!model.hasAirlineDesignator() && !model.getLivery().hasValidDbKey())
            {
                // if there is no livery (normal) we need an airline
                this->addStatusMessage(CStatusMessage(this->categgories(), CStatusMessage::SeverityError, "No airline designator"), model);
                this->m_noData++;
            }
            else
            {
                // stash here consolidates with DB data and validates
                CAircraftModel stashModel(this->getMappingComponent()->consolidateModel(model));
                CStatusMessageList validationMsgs(stashModel.validate(true));
                validationMsgs.removeWarningsAndBelow();
                CStatusMessage msg = validationMsgs.toSingleMessage();
                if (msg.getSeverity() == CStatusMessage::SeverityError)
                {
                    this->m_noValidationFailed++;
                }
                else
                {
                    msg = CStatusMessage(categgories(), CStatusMessage::SeverityInfo, "Stashed succesfully");
                    stashed = true;
                    this->m_noStashed++;
                    model = stashModel;
                }
                this->addStatusMessage(msg, stashModel);
            }
            return stashed;
        }

        const CLogCategoryList &CDbAutoStashingComponent::categgories()
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::guiComponent() }).join({ CLogCategory::mapping()}));
            return cats;
        }
    } // ns
} // ns
