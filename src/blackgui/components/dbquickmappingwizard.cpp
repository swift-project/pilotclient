/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "dbquickmappingwizard.h"
#include "ui_dbquickmappingwizard.h"
#include "dbaircrafticaoselectorcomponent.h"
#include "dbairlinenameselectorcomponent.h"
#include "blackgui/views/aircrafticaoview.h"
#include "blackgui/views/liveryview.h"
#include "blackgui/views/distributorview.h"
#include "blackgui/uppercasevalidator.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/db/databasewriter.h"
#include "blackcore/webdataservices.h"
#include "blackconfig/buildconfig.h"

#include <QStringBuilder>

using namespace BlackCore;
using namespace BlackCore::Db;
using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Components
    {
        CDbQuickMappingWizard::CDbQuickMappingWizard(QWidget *parent) :
            QWizard(parent),
            ui(new Ui::CDbQuickMappingWizard)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
            Q_ASSERT_X(sGui->hasWebDataServices(), Q_FUNC_INFO, "Missing web services");
            Q_ASSERT_X(sGui->getWebDataServices()->getDatabaseWriter(), Q_FUNC_INFO, "Missing writer");

            ui->setupUi(this);
            ui->selector_AircraftIcaoCode->setFocus();
            ui->selector_AircraftIcaoCode->displayWithIcaoDescription(false);
            ui->selector_AircraftIcaoCode->displayMode(CDbAircraftIcaoSelectorComponent::DisplayCompleterString);
            ui->selector_AirlineIcaoCode->displayWithIcaoDescription(false);
            ui->selector_Simulator->setNoSelectionMeansAll(false);
            ui->selector_Simulator->setRememberSelection(false);
            ui->editor_AircraftModel->allowDrop(false);
            ui->editor_AircraftModel->setReadOnly(true);
            CGuiUtility::checkBoxReadOnly(ui->cb_Military, true);
            CGuiUtility::checkBoxReadOnly(ui->cb_VirtualAirline, true);

            connect(sGui->getWebDataServices(), &CWebDataServices::swiftDbAllDataRead, this, &CDbQuickMappingWizard::onWebDataRead, Qt::QueuedConnection);
            connect(sGui->getWebDataServices()->getDatabaseWriter(), &CDatabaseWriter::publishedModels, this, &CDbQuickMappingWizard::onPublishedModels, Qt::QueuedConnection);

            connect(this, &CDbQuickMappingWizard::currentIdChanged, this, &CDbQuickMappingWizard::currentWizardPageChanged);
            connect(ui->selector_AircraftIcaoCode, &CDbAircraftIcaoSelectorComponent::changedAircraftIcao, this, &CDbQuickMappingWizard::onAircraftSelected, Qt::QueuedConnection);
            connect(ui->selector_AirlineIcaoCode, &CDbAirlineIcaoSelectorComponent::changedAirlineIcao, this, &CDbQuickMappingWizard::onAirlineSelected, Qt::QueuedConnection);
            connect(ui->selector_AirlineName, &CDbAirlineIcaoSelectorComponent::changedAirlineIcao, this, &CDbQuickMappingWizard::onAirlineSelected, Qt::QueuedConnection);

            ui->comp_Log->showFilterDialog(); // filter for log normally not needed, so dialog (not bar)
            CGuiUtility::setWizardButtonWidths(this);

            // init if data already available
            this->onWebDataRead();
        }

        CDbQuickMappingWizard::~CDbQuickMappingWizard()
        { }

        const QStringList &CDbQuickMappingWizard::getLogCategories()
        {
            static const QStringList cats { CLogCategories::mapping(), CLogCategories::guiComponent() };
            return cats;
        }

        void CDbQuickMappingWizard::keyPressEvent(QKeyEvent *event)
        {
            Qt::Key key = static_cast<Qt::Key>(event->key());
            if (key == Qt::Key_Enter || key == Qt::Key_Return)
            {
                // disable enter, interferes with filter returnPressed
                event->accept();
            }
            else
            {
                QWizard::keyPressEvent(event);
            }
        }

        void CDbQuickMappingWizard::presetAircraftIcao(const CAircraftIcaoCode &aircraftIcao)
        {
            this->clear();
            ui->selector_AircraftIcaoCode->setAircraftIcao(aircraftIcao);
            ui->selector_AircraftIcaoCode->setFocus();
        }

        void CDbQuickMappingWizard::presetModel(const CAircraftModel &model)
        {
            QString ms = model.getModelString();
            if (!model.getDescription().isEmpty())
            {
                ms += u" (" % model.getDescription() % u")";
            }

            this->presetAircraftIcao(model.getAircraftIcaoCode());
            ui->selector_AirlineIcaoCode->setAirlineIcao(model.getAirlineIcaoCode());
            ui->selector_AirlineName->setAirlineIcao(model.getAirlineIcaoCode());
            ui->comp_Distributor->view()->selectDbKey(model.getDistributor().getDbKey());
            ui->le_ModelString->setText(model.hasModelString() ? ms : "<no model string>");
            ui->selector_AircraftIcaoCode->setFocus();

            const CLivery livery(model.getLivery());
            if (livery.isColorLivery())
            {
                ui->comp_ColorSearch->presetColorLivery(livery);
                ui->rb_ColorLivery->setChecked(true);
            }

            m_model = model;
        }

        void CDbQuickMappingWizard::clear()
        {
            m_lastId = 0;
            ui->editor_AircraftModel->clear();
            ui->comp_Log->clear();
            this->restart();
        }

        void CDbQuickMappingWizard::setAircraftIcaoFilter()
        {
            const CAircraftIcaoCode icao(ui->selector_AircraftIcaoCode->getAircraftIcao());
            if (icao.isLoadedFromDb())
            {
                ui->comp_AircraftIcao->view()->sortByPropertyIndex(CAircraftIcaoCode::IndexRank);
                ui->comp_AircraftIcao->filter(icao);
                ui->comp_AircraftIcao->selectAircraftIcao(icao);
            }
        }

        void CDbQuickMappingWizard::setAirlineIcaoFilter()
        {
            const CAirlineIcaoCode icao(ui->selector_AirlineIcaoCode->getAirlineIcao());
            if (icao.isLoadedFromDb())
            {
                ui->comp_Livery->view()->sortByPropertyIndex(CLivery::IndexDbIntegerKey);
                ui->comp_Livery->filterByAirline(icao);
                ui->comp_Livery->view()->selectRow(0);
            }
        }

        void CDbQuickMappingWizard::setColorFilter()
        {
            const CLivery colorLivery(ui->comp_ColorSearch->getLivery());
            if (colorLivery.isLoadedFromDb())
            {
                ui->comp_Livery->filter(colorLivery);
                ui->comp_Livery->view()->selectRow(0);
            }
        }

        void CDbQuickMappingWizard::setDistributorFilter()
        {
            const CSimulatorInfo sims = this->guessSimulator();
            ui->comp_Distributor->filterBySimulator(sims);
            if (m_model.getDistributor().isLoadedFromDb())
            {
                const bool s = ui->comp_Distributor->selectDistributor(m_model.getDistributor());
                Q_UNUSED(s);
            }
        }

        CLivery CDbQuickMappingWizard::getFirstSelectedOrDefaultLivery() const
        {
            const CLivery l = ui->comp_Livery->view()->firstSelectedOrDefaultObject();
            if (l.isLoadedFromDb()) { return l; }
            if (m_model.getLivery().isLoadedFromDb()) { return m_model.getLivery(); }
            return l;
        }

        CAircraftIcaoCode CDbQuickMappingWizard::getFirstSelectedOrDefaultAircraftIcao() const
        {
            const CAircraftIcaoCode icao = ui->comp_AircraftIcao->view()->firstSelectedOrDefaultObject();
            if (icao.isLoadedFromDb()) { return icao; }
            if (m_model.getAircraftIcaoCode().isLoadedFromDb()) { return m_model.getAircraftIcaoCode(); }
            return icao;
        }

        BlackMisc::Simulation::CDistributor CDbQuickMappingWizard::getFirstSelectedOrDefaultDistributor() const
        {
            const CDistributor dist = ui->comp_Distributor->view()->firstSelectedOrDefaultObject();
            if (dist.isLoadedFromDb()) { return dist; }
            if (m_model.getDistributor().isLoadedFromDb()) { return m_model.getDistributor(); }
            return dist;
        }

        void CDbQuickMappingWizard::onWebDataRead()
        {
            if (!sGui || !sGui->hasWebDataServices()) { return; }
        }

        void CDbQuickMappingWizard::onPublishedModels(const CAircraftModelList &modelsPublished, const CAircraftModelList &modelsSkipped, const CStatusMessageList &messages, bool requestSuccessful, bool directWrite)
        {
            Q_UNUSED(modelsPublished);
            Q_UNUSED(modelsSkipped);
            Q_UNUSED(directWrite);
            CStatusMessageList msgs;
            if (requestSuccessful)
            {
                msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityInfo, u"Publishing request sent"));
            }
            else
            {
                msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityError, u"Publishing request failed"));
            }
            msgs.push_back(messages);
            ui->comp_Log->appendStatusMessagesToList(msgs);
        }

        void CDbQuickMappingWizard::currentWizardPageChanged(int id)
        {
            const bool forward = id > m_lastId;
            const bool colorMode = ui->rb_ColorLivery->isChecked();
            m_lastId = id;

            const Pages page = static_cast<Pages>(id);
            switch (page)
            {
            case PageAircraftSelect:
                {
                    this->setAircraftIcaoFilter();
                }
                break;
            case PageColor:
                if (!colorMode)
                {
                    forward ? this->next() : this->back();
                }
                break;
            case PageLiverySelect:
                {
                    if (colorMode)
                    {
                        this->setColorFilter();
                    }
                    else
                    {
                        this->setAirlineIcaoFilter();
                    }
                }
                break;
            case PageDistributorSelect:
                {
                    this->setDistributorFilter();
                }
                break;
            case PageConfirmation:
                {
                    const CSimulatorInfo sims = this->guessSimulator();
                    ui->selector_Simulator->setValue(sims);
                    ui->editor_AircraftModel->setLivery(this->getFirstSelectedOrDefaultLivery());
                    ui->editor_AircraftModel->setDistributor(this->getFirstSelectedOrDefaultDistributor());
                    ui->editor_AircraftModel->setAircraftIcao(this->getFirstSelectedOrDefaultAircraftIcao());
                    const CStatusMessageList msgs(this->validateData());
                    const bool errorFree = !msgs.hasWarningOrErrorMessages();
                    ui->fr_ConfirmationOk->setVisible(errorFree);
                    ui->fr_ConfirmationStillErrors->setVisible(!errorFree);
                    if (!errorFree)
                    {
                        ui->wp6_Confirmation->showOverlayMessages(msgs);
                    }
                }
                break;
            case PageCredentials:
                {
                    ui->comp_Log->clear();
                }
                break;
            case PageLastConfirmation:
                {
                    // void
                }
                break;
            case PageSendStatus:
                {
                    this->writeModelToDb();
                    this->button(BackButton)->hide();
                }
                break;
            default:
                break;
            }
        }

        bool CDbQuickMappingWizard::validateCurrentPage()
        {
            const Pages page = static_cast<Pages>(this->currentId());
            bool ok = false;
            switch (page)
            {
            case PageConfirmation:
                {
                    const CStatusMessageList msgs(this->validateData());
                    if (!msgs.isEmpty())
                    {
                        ui->wp6_Confirmation->showOverlayMessages(msgs);
                    }
                    ok = !msgs.hasWarningOrErrorMessages();
                }
                break;
            case PageCredentials:
                {
                    ok = ui->comp_DbLogin->isUserAuthenticated();
                    if (!ok)
                    {
                        ui->wp7_Credentials->showOverlayHTMLMessage("No user credentials, read login hints!", 10 * 1000);
                    }
                }
                break;
            default:
                {
                    ok = true;
                }
                break;
            }

            return ok;
        }

        CStatusMessageList CDbQuickMappingWizard::validateData() const
        {
            CStatusMessageList msgs(ui->editor_AircraftModel->validate(true));
            if (ui->le_ModelString->text().isEmpty())
            {
                const CStatusMessage error(this, CStatusMessage::SeverityError, u"Missing model string", true);
                msgs.push_back(error);
            }

            const CStatusMessage vMsg = ui->selector_Simulator->getValue().validateSimulatorsForModel();
            if (vMsg.isWarningOrAbove())
            {
                msgs.push_back(vMsg);
            }

            return msgs;
        }

        void CDbQuickMappingWizard::consolidateModelWithUIData()
        {
            CAircraftModel model = m_model;
            model.setAircraftIcaoCode(ui->editor_AircraftModel->getAircraftIcao());
            model.setDistributor(ui->editor_AircraftModel->getDistributor());
            model.setLivery(ui->editor_AircraftModel->getLivery());
            model.setSimulator(ui->selector_Simulator->getValue());
            m_model = model;
        }

        CSimulatorInfo CDbQuickMappingWizard::guessSimulator() const
        {
            CSimulatorInfo sims = m_model.getSimulator();
            if (!sims.isAnySimulator() && m_model.hasModelString() && sGui && sGui->hasWebDataServices())
            {
                const CAircraftModel m = sGui->getWebDataServices()->getModelForModelString(m_model.getModelString());
                if (m.isLoadedFromDb()) { sims = m.getSimulator(); }
            }

            if (sGui && !sGui->isShuttingDown() && sGui->getIContextSimulator() && sGui->getIContextSimulator()->isSimulatorAvailable())
            {
                sims.add(sGui->getIContextSimulator()->getSimulatorPluginInfo().getSimulator());
            }
            return sims;
        }

        void CDbQuickMappingWizard::writeModelToDb()
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices() || !sGui->getWebDataServices()->getDatabaseWriter()) { return; }
            this->consolidateModelWithUIData();

            // make sure the model is correctly excluded for XP etc.
            static const QString qmw(u"[swift QMW " % CBuildConfig::getVersionString() % u"]");
            if (m_model.getSimulator().isXPlane()) { m_model.setModelMode(CAircraftModel::Exclude); }
            m_model.setVersion(CBuildConfig::getVersionString());
            const QString extraInfo = QString(m_model.getDescription() % u" " % qmw).simplified().trimmed();

            const CStatusMessageList msgs = sGui->getWebDataServices()->getDatabaseWriter()->asyncPublishModel(m_model, extraInfo);
            ui->comp_Log->appendStatusMessagesToList(msgs);
        }

        void CDbQuickMappingWizard::onAirlineSelected(const CAirlineIcaoCode &icao)
        {
            if (icao.isLoadedFromDb())
            {
                ui->cb_VirtualAirline->setChecked(icao.isVirtualAirline());
                ui->cb_Military->setChecked(icao.isMilitary());
                ui->selector_AirlineName->setAirlineIcao(icao);
                ui->selector_AirlineIcaoCode->setAirlineIcao(icao);
                // already trigger sorting, if sorting is already correct it does nothing
                // avoids issue with later selection overridden by sorting/filtering
                this->setAirlineIcaoFilter();
            }
        }

        void CDbQuickMappingWizard::onAircraftSelected(const CAircraftIcaoCode &icao)
        {
            if (icao.isLoadedFromDb())
            {
                ui->cb_Military->setChecked(icao.isMilitary());
                // already trigger sorting, if sorting is already correct it does nothing
                // avoids issue with later selection overidden by sorting/filtering
                this->setAircraftIcaoFilter();
            }
        }
    } // ns
} // ns
