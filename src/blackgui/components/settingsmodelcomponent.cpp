/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "settingsmodelcomponent.h"
#include "ui_settingsmodelcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/db/backgrounddataupdater.h"
#include "blackmisc/logmessage.h"

#include <QValidator>

using namespace BlackMisc;
using namespace BlackMisc::Simulation::Settings;
using namespace BlackGui::Settings;
using namespace BlackCore::Db;

namespace BlackGui::Components
{
    CSettingsModelComponent::CSettingsModelComponent(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CSettingsModelComponent)
    {
        ui->setupUi(this);
        ui->le_ConsolidateSecs->setValidator(new QIntValidator(0, TBackgroundConsolidation::maxSecs(), ui->le_ConsolidateSecs));

        this->cacheChanged();
        const QString lbl("Consolidate (%1-%2s):");
        ui->lbl_Consolidate->setText(lbl.arg(TBackgroundConsolidation::minSecs()).arg(TBackgroundConsolidation::maxSecs()));

        connect(ui->le_ConsolidateSecs, &QLineEdit::returnPressed, this, &CSettingsModelComponent::consolidationEntered);
        connect(ui->cb_AllowExcludeModels, &QCheckBox::toggled, this, &CSettingsModelComponent::allowExcludedModelsChanged);

        // start updater if not yet done
        QPointer<CSettingsModelComponent> myself(this);
        QTimer::singleShot(2500, this, [ = ]
        {
            if (!myself) { return; }
            this->consolidationEntered();
        });
    }

    CSettingsModelComponent::~CSettingsModelComponent()
    { }

    int CSettingsModelComponent::getBackgroundUpdaterIntervallSecs() const
    {
        const QString v = ui->le_ConsolidateSecs->text().trimmed();
        if (v.isEmpty()) { return -1; }
        bool ok = false;
        const int secs = v.toInt(&ok);
        return ok ? secs : -1;
    }

    void CSettingsModelComponent::setBackgroundUpdater(const CBackgroundDataUpdater *updater)
    {
        m_updater = updater;
    }

    void CSettingsModelComponent::consolidationEntered()
    {
        int v = this->getBackgroundUpdaterIntervallSecs();
        if (v < TBackgroundConsolidation::minSecs()) { v = -1; }

        const CStatusMessage m = m_consolidationSetting.setAndSave(v);
        CLogMessage::preformatted(m);
        this->cacheChanged();
    }

    void CSettingsModelComponent::allowExcludedModelsChanged(bool allow)
    {
        CModelSettings ms = m_modelSettings.get();
        if (ms.getAllowExcludedModels() == allow) { return; }
        ms.setAllowExcludedModels(allow);
        const CStatusMessage msg = m_modelSettings.setAndSave(ms);
        CLogMessage::preformatted(msg);
    }

    void CSettingsModelComponent::cacheChanged()
    {
        const int v = m_consolidationSetting.get();
        const bool on = v > 0;
        const QString s = on ? QString::number(v) : "";
        ui->le_ConsolidateSecs->setText(s);

        const bool updater =
            on &&
            sApp && !sApp->isShuttingDown() &&
            m_updater && m_updater->isEnabled();
        ui->comp_Led->setOn(updater);

        // avoid unnecessary roundtrips
        const CModelSettings ms = m_modelSettings.get();
        if (ui->cb_AllowExcludeModels->isChecked() != ms.getAllowExcludedModels())
        {
            ui->cb_AllowExcludeModels->setChecked(ms.getAllowExcludedModels());
        }
    }
} // ns
