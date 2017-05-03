/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingsmodelcomponent.h"
#include "ui_settingsmodelcomponent.h"

#include "blackmisc/logmessage.h"
#include <QValidator>

using namespace BlackMisc;
using namespace BlackGui::Settings;

namespace BlackGui
{
    namespace Components
    {
        CSettingsModelComponent::CSettingsModelComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsModelComponent)
        {
            ui->setupUi(this);
            ui->le_ConsolidateSecs->setValidator(new QIntValidator(0, TBackgroundConsolidation::maxSecs(), ui->le_ConsolidateSecs));

            this->cacheChanged();
            connect(ui->le_ConsolidateSecs, &QLineEdit::returnPressed, this, &CSettingsModelComponent::consolidationEntered);
            static QString lbl("Consolidate (%1-%2s):");
            ui->lbl_Consolidate->setText(lbl.arg(TBackgroundConsolidation::minSecs()).arg(TBackgroundConsolidation::maxSecs()));
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

        void CSettingsModelComponent::consolidationEntered()
        {
            int v = this->getBackgroundUpdaterIntervallSecs();
            if (v < TBackgroundConsolidation::minSecs()) v = -1;

            const CStatusMessage m = m_consolidationSetting.setAndSave(v);
            CLogMessage::preformatted(m);
            this->cacheChanged();
        }

        void CSettingsModelComponent::cacheChanged()
        {
            const int v = m_consolidationSetting.get();
            const bool on = v > 0;
            const QString s = on ? QString::number(v) : "";
            ui->le_ConsolidateSecs->setText(s);
            ui->comp_Led->setOn(on);
        }
    } // ns
} // ns
