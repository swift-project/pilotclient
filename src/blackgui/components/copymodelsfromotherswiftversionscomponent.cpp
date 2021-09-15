/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "copymodelsfromotherswiftversionscomponent.h"
#include "ui_copymodelsfromotherswiftversionscomponent.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackcore/application.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"

#include <QSet>
#include <QPointer>
#include <QFileInfo>
#include <QMessageBox>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Models;

namespace BlackGui::Components
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
        const CSimulatorInfo selectedSimulators = ui->comp_SimulatorSelector->getValue();
        const QSet<CSimulatorInfo> simulators = selectedSimulators.asSingleSimulatorSet();
        if (simulators.isEmpty())
        {
            static const CStatusMessage m = CStatusMessage(this).validationError(u"No simulator(s) selected");
            this->showOverlayMessage(m);
            return;
        }

        const bool set = ui->cb_ModelSet->isChecked();
        const bool cache = ui->cb_ModelCache->isChecked();
        if (!cache && !set)
        {
            static const CStatusMessage m = CStatusMessage(this).validationError(u"No models selected (cache? model set?)");
            this->showOverlayMessage(m);
            return;
        }

        if (!ui->comp_OtherSwiftVersions->hasSelection())
        {
            static const CStatusMessage m = CStatusMessage(this).validationError(u"No other version selected");
            this->showOverlayMessage(m);
            return;
        }

        int sets = 0;
        int caches = 0;
        const CApplicationInfo otherVersion = ui->comp_OtherSwiftVersions->selectedOtherVersion();
        for (const CSimulatorInfo &simulator : simulators)
        {
            if (set)
            {
                // inits current version cache
                m_modelSetCaches.synchronizeCache(simulator);

                // get file name
                CAircraftModelList otherSet;
                const QString thisVersionModelSetFile = m_modelSetCaches.getFilename(simulator);
                if (this->readDataFile(thisVersionModelSetFile, otherSet, otherVersion, simulator) && !otherSet.isEmpty())
                {
                    CApplication::processEventsFor(250);
                    if (this->confirmOverride(QStringLiteral("Override model set for '%1'").arg(simulator.toQString())))
                    {
                        m_modelSetCaches.setModelsForSimulator(otherSet, simulator);
                    }
                }
                sets++;
            } // set

            if (cache)
            {
                // inits current version cache
                m_modelCaches.synchronizeCache(simulator);

                // get file name
                CAircraftModelList otherCache;
                const QString thisVersionModelCacheFile = m_modelCaches.getFilename(simulator);
                if (this->readDataFile(thisVersionModelCacheFile, otherCache, otherVersion, simulator) && !otherCache.isEmpty())
                {
                    CApplication::processEventsFor(250);
                    if (this->confirmOverride(QStringLiteral("Override model cache for '%1'").arg(simulator.toQString())))
                    {
                        m_modelCaches.setModelsForSimulator(otherCache, simulator);
                    }
                }
                caches++;
            }
        } // all sims

        if (sets > 0 || caches > 0)
        {
            const CStatusMessage m = CStatusMessage(this).validationInfo(u"Copied %1 sets and %2 caches for '%3'") << sets << caches << selectedSimulators.toQString(true);
            this->showOverlayHTMLMessage(m, 7500);
        }
    }

    bool CCopyModelsFromOtherSwiftVersionsComponent::readDataFile(const QString &thisVersionModelFile, CAircraftModelList &models, const CApplicationInfo &otherVersion, const CSimulatorInfo &sim)
    {
        // init
        models.clear();

        // create relative file name
        QString relativeModelFile = thisVersionModelFile;
        relativeModelFile = relativeModelFile.replace(CSwiftDirectories::applicationDataDirectory(), "", Qt::CaseInsensitive);
        if (relativeModelFile.length() < 2) { return false; }
        relativeModelFile = relativeModelFile.mid(relativeModelFile.indexOf('/', 1));

        const QString otherModelFile = CFileUtils::appendFilePathsAndFixUnc(otherVersion.getApplicationDataDirectory(), relativeModelFile);
        const QFileInfo fiOtherModelFile(otherModelFile);
        if (!fiOtherModelFile.exists())
        {
            ui->le_Status->setText(QStringLiteral("No models here: '%1'").arg(fiOtherModelFile.absoluteFilePath()));
            return false;
        }

        // read other file
        const QString jsonString = CFileUtils::readFileToString(fiOtherModelFile.absoluteFilePath());
        if (jsonString.isEmpty()) { return false; }
        try
        {
            models = CAircraftModelList::fromMultipleJsonFormats(jsonString);
            ui->tvp_AircraftModels->updateContainerAsync(models);
            ui->le_Status->setText(QStringLiteral("Imported models: '%1'").arg(fiOtherModelFile.absoluteFilePath()));
        }
        catch (const CJsonException &ex)
        {
            this->showOverlayMessage(CStatusMessage::fromJsonException(ex, this, QStringLiteral("JSON format error. '%1'").arg(fiOtherModelFile.absoluteFilePath())));
            return false;
        }

        ui->le_Status->setText(QStringLiteral("Imported %1 models '%2' for %3").arg(models.size()).arg(fiOtherModelFile.fileName(), sim.toQString()));
        return true;
    }

    bool CCopyModelsFromOtherSwiftVersionsComponent::confirmOverride(const QString &msg)
    {
        if (!sApp || sApp->isShuttingDown()) { return false; }
        if (ui->cb_Silent->isChecked())
        {
            // allow UI updates
            sApp->processEventsFor(50);
            return true;
        }
        const QMessageBox::StandardButton reply = QMessageBox::question(this, QStringLiteral("Confirm override"), withQuestionMark(msg), QMessageBox::Yes | QMessageBox::No);
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

    void CCopyModelsFromOtherSwiftVersionsComponent::reloadOtherVersions(int deferMs)
    {
        ui->comp_OtherSwiftVersions->reloadOtherVersionsDeferred(deferMs);
    }

    void CCopyModelsFromOtherSwiftVersionsWizardPage::initializePage()
    {
        // force reload as the other version could be changed
        if (m_copyModels) { m_copyModels->reloadOtherVersions(1000); }
    }

    bool CCopyModelsFromOtherSwiftVersionsWizardPage::validatePage()
    {
        Q_ASSERT_X(m_copyModels, Q_FUNC_INFO, "Missing widget");
        return true;
    }
} // ns
