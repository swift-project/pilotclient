/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "applicationclosedialog.h"
#include "ui_applicationclosedialog.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackcore/context/contextapplication.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/logmessage.h"
#include <QStringListModel>
#include <QModelIndexList>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace BlackMisc;
using namespace BlackGui;

namespace BlackGui
{
    namespace Components
    {
        CApplicationCloseDialog::CApplicationCloseDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CApplicationCloseDialog)
        {
            ui->setupUi(this);
            this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
            this->initSettingsView();
            ui->bb_ApplicationCloseDialog->button(QDialogButtonBox::Save)->setDefault(true);

            connect(this, &CApplicationCloseDialog::accepted, this, &CApplicationCloseDialog::ps_onAccepted);
            connect(this, &CApplicationCloseDialog::rejected, this, &CApplicationCloseDialog::ps_onRejected);
            connect(ui->bb_ApplicationCloseDialog, &QDialogButtonBox::clicked, this, &CApplicationCloseDialog::ps_buttonClicked);
        }

        CApplicationCloseDialog::~CApplicationCloseDialog()
        { }

        void CApplicationCloseDialog::ps_onAccepted()
        {
            const QModelIndexList indexes = ui->lv_UnsavedSettings->selectionModel()->selectedIndexes();
            if (indexes.isEmpty()) { return; }
            const QList<int> rows = CGuiUtility::indexToUniqueRows(indexes);
            QStringList saveKeys;
            for (int r : rows)
            {
                const QString key = this->m_settingskeys.at(r);
                saveKeys.append(key);
            }
            CStatusMessage msg = sApp->saveSettingsByKey(saveKeys);
            if (msg.isFailure()) { CLogMessage::preformatted(msg); }
        }

        void CApplicationCloseDialog::ps_onRejected()
        {
            // void
        }

        void CApplicationCloseDialog::ps_buttonClicked(QAbstractButton *button)
        {
            if (button == ui->bb_ApplicationCloseDialog->button(QDialogButtonBox::Discard))
            {
                ui->lv_UnsavedSettings->clearSelection();
                this->accept(); // fake accept with unselected items
            }
        }

        void CApplicationCloseDialog::initSettingsView()
        {
            QStringList settings(sApp->getIContextApplication()->getUnsavedSettingsKeys());
            settings.sort();
            QStringListModel *model = new QStringListModel(settings, this);
            ui->lv_UnsavedSettings->setModel(model);
            ui->lv_UnsavedSettings->selectAll();

            this->m_settingskeys = settings;
        }
    } // ns
} // ns
