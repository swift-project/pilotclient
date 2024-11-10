// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "applicationclosedialog.h"
#include "ui_applicationclosedialog.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "core/context/contextapplication.h"
#include "misc/settingscache.h"
#include "misc/logmessage.h"
#include <QStringListModel>
#include <QModelIndexList>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace swift::misc;
using namespace BlackGui;
using namespace swift::core::context;

namespace BlackGui::Components
{
    CApplicationCloseDialog::CApplicationCloseDialog(QWidget *parent) : QDialog(parent),
                                                                        ui(new Ui::CApplicationCloseDialog)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");

        ui->setupUi(this);
        this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        this->initSettingsView();
        ui->bb_ApplicationCloseDialog->button(QDialogButtonBox::Save)->setDefault(true);

        connect(this, &CApplicationCloseDialog::accepted, this, &CApplicationCloseDialog::onAccepted);
        connect(this, &CApplicationCloseDialog::rejected, this, &CApplicationCloseDialog::onRejected);
        connect(ui->bb_ApplicationCloseDialog, &QDialogButtonBox::clicked, this, &CApplicationCloseDialog::buttonClicked);
    }

    CApplicationCloseDialog::~CApplicationCloseDialog()
    {}

    void CApplicationCloseDialog::onAccepted()
    {
        const QModelIndexList indexes = ui->lv_UnsavedSettings->selectionModel()->selectedIndexes();
        if (indexes.isEmpty()) { return; }
        QStringList saveKeys;
        const QList<int> rows = CGuiUtility::indexToUniqueRows(indexes);
        for (int r : rows)
        {
            const QString description = m_settingsDescriptions[r];
            const QString key = m_settingsDictionary.key(description);
            if (!key.isEmpty()) { saveKeys.append(key); }
        }
        if (saveKeys.isEmpty()) { return; }
        const CStatusMessage msg = sApp->saveSettingsByKey(saveKeys);
        if (msg.isFailure()) { CLogMessage::preformatted(msg); }
    }

    void CApplicationCloseDialog::onRejected()
    {
        // void
    }

    void CApplicationCloseDialog::buttonClicked(QAbstractButton *button)
    {
        if (button == ui->bb_ApplicationCloseDialog->button(QDialogButtonBox::Discard))
        {
            ui->lv_UnsavedSettings->clearSelection();
            this->accept(); // fake accept with unselected items
        }
    }

    void CApplicationCloseDialog::initSettingsView()
    {
        const CSettingsDictionary settingsDictionary(sApp->getIContextApplication()->getUnsavedSettingsKeysDescribed());
        QStringList descriptions = settingsDictionary.values();
        descriptions.sort();
        QStringListModel *model = new QStringListModel(descriptions, this);
        ui->lv_UnsavedSettings->setModel(model);
        ui->lv_UnsavedSettings->selectAll();

        m_settingsDictionary = settingsDictionary;
        m_settingsDescriptions = descriptions;
    }
} // ns
