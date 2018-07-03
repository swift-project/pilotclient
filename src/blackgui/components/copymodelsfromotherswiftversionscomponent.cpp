/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "copymodelsfromotherswiftversionscomponent.h"
#include "ui_copymodelsfromotherswiftversionscomponent.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackcore/application.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/directoryutils.h"

#include <QSet>
#include <QFileInfo>
#include <QMessageBox>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Components
    {
        CCopyModelsFromOtherSwiftVersionsComponent::CCopyModelsFromOtherSwiftVersionsComponent(QWidget *parent) :
            COverlayMessagesFrame(parent),
            ui(new Ui::CCopyModelsFromOtherSwiftVersionsComponent)
        {
            ui->setupUi(this);
            ui->comp_SimulatorSelector->setMode(CSimulatorSelector::CheckBoxes);
            ui->comp_SimulatorSelector->clear();
            ui->tvp_AircraftModels->setAircraftModelMode(CAircraftModelListModel::OwnModelSet);
            connect(ui->pb_StartCopying, &QPushButton::clicked, this, &CCopyModelsFromOtherSwiftVersionsComponent::copy);
            connect(ui->comp_OtherSwiftVersions, &COtherSwiftVersionsComponent::versionChanged, this, &CCopyModelsFromOtherSwiftVersionsComponent::onVersionChanged);
        }

        CCopyModelsFromOtherSwiftVersionsComponent::~CCopyModelsFromOtherSwiftVersionsComponent()
        { }

        void CCopyModelsFromOtherSwiftVersionsComponent::copy()
        {
            const QSet<CSimulatorInfo> sims = ui->comp_SimulatorSelector->getValue().asSingleSimulatorSet();
            if (sims.isEmpty())
            {
                static const CStatusMessage m = CStatusMessage(this).validationError("No simulators selected");
                this->showOverlayMessage(m);
                return;
            }

            const bool set = ui->cb_ModelSet->isChecked();
            const bool cache = ui->cb_ModelCache->isChecked();
            if (!cache && !set)
            {
                static const CStatusMessage m = CStatusMessage(this).validationError("No simulators selected");
                this->showOverlayMessage(m);
                return;
            }

            if (!ui->comp_OtherSwiftVersions->hasSelection())
            {
                static const CStatusMessage m = CStatusMessage(this).validationError("No other version selected");
                this->showOverlayMessage(m);
                return;
            }

            const CApplicationInfo otherVersion = ui->comp_OtherSwiftVersions->selectedOtherVersion();
            for (const CSimulatorInfo &sim : sims)
            {
                if (set)
                {
                    // inits current version cache
                    m_modelSetCaches.setCurrentSimulator(sim);
                    m_modelSetCaches.synchronizeCurrentCache();

                    // get file name
                    CAircraftModelList otherSet;
                    const QString thisVersionModelSetFile = m_modelSetCaches.getFilename(sim);
                    if (this->readDataFile(thisVersionModelSetFile, otherSet, otherVersion, sim) && !otherSet.isEmpty())
                    {
                        CApplication::processEventsFor(250);
                        if (this->confirmOverride(QString("Override model set for '%1'").arg(sim.toQString())))
                        {
                            m_modelSetCaches.setModels(otherSet, sim);
                        }
                    }
                } // set

                if (cache)
                {
                    // inits current version cache
                    m_modelCaches.setCurrentSimulator(sim);
                    m_modelCaches.synchronizeCurrentCache();

                    // get file name
                    CAircraftModelList otherCache;
                    const QString thisVersionModelCacheFile = m_modelCaches.getFilename(sim);
                    if (this->readDataFile(thisVersionModelCacheFile, otherCache, otherVersion, sim) && !otherCache.isEmpty())
                    {
                        CApplication::processEventsFor(250);
                        if (this->confirmOverride(QString("Override model cache for '%1'").arg(sim.toQString())))
                        {
                            m_modelCaches.setModels(otherCache, sim);
                        }
                    }
                }
            } // all sims
        }

        bool CCopyModelsFromOtherSwiftVersionsComponent::readDataFile(const QString &thisVersionModelFile, CAircraftModelList &models, const CApplicationInfo &otherVersion, const CSimulatorInfo &sim)
        {
            // init
            models.clear();

            // create relative file name
            QString relativeModelFile = thisVersionModelFile;
            relativeModelFile = relativeModelFile.replace(CDirectoryUtils::applicationDataDirectory(), "", Qt::CaseInsensitive);
            if (relativeModelFile.length() < 2) { return false; }
            relativeModelFile = relativeModelFile.mid(relativeModelFile.indexOf('/', 1));

            const QString otherModelFile = CFileUtils::appendFilePathsAndFixUnc(otherVersion.getApplicationDataDirectory(), relativeModelFile);
            const QFileInfo fiOtherModelFile(otherModelFile);
            if (!fiOtherModelFile.exists())
            {
                static const QString noSet("No models here: '%1'");
                ui->le_Status->setText(noSet.arg(fiOtherModelFile.absoluteFilePath()));
                return false;
            }

            // read other file
            const QString jsonString = CFileUtils::readFileToString(fiOtherModelFile.absoluteFilePath());
            if (jsonString.isEmpty()) { return false; }
            try
            {
                models = CAircraftModelList::fromMultipleJsonFormats(jsonString);
                ui->tvp_AircraftModels->updateContainerAsync(models);
                static const QString importSet("Imported models: '%1'");
                ui->le_Status->setText(importSet.arg(fiOtherModelFile.absoluteFilePath()));
            }
            catch (const CJsonException &ex)
            {
                static const QString m("JSON format error. '%1'");
                this->showOverlayMessage(ex.toStatusMessage(this, m.arg(fiOtherModelFile.absoluteFilePath())));
                return false;
            }

            static const QString importSet("Imported %1 models '%2' for %3");
            ui->le_Status->setText(importSet.arg(models.size()).arg(fiOtherModelFile.fileName(), sim.toQString()));
            return true;
        }

        bool CCopyModelsFromOtherSwiftVersionsComponent::confirmOverride(const QString &msg)
        {
            if (ui->cb_Silent->isChecked()) { return true; }
            const QMessageBox::StandardButton reply = QMessageBox::question(this, QStringLiteral("Confirm override"), withQUestionMark(msg), QMessageBox::Yes | QMessageBox::No);
            return reply == QMessageBox::Yes;
        }

        void CCopyModelsFromOtherSwiftVersionsComponent::onVersionChanged(const CApplicationInfo &otherVersion)
        {
            const CSimulatorInfo cacheSims = m_modelCaches.otherVersionSimulatorsWithFile(otherVersion);
            const CSimulatorInfo setSims = m_modelSetCaches.otherVersionSimulatorsWithFile(otherVersion);

            ui->cb_ModelCache->setChecked(cacheSims.isAnySimulator());
            ui->cb_ModelSet->setChecked(setSims.isAnySimulator());
            ui->comp_SimulatorSelector->setValue(setSims);
        }

        bool CCopyModelsFromOtherSwiftVersionsWizardPage::validatePage()
        {
            Q_ASSERT_X(m_copyModels, Q_FUNC_INFO, "Missing widget");
            return true;
        }
    } // ns
} // ns
