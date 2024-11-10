// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "modelconverterxsetupcomponent.h"
#include "ui_modelconverterxsetupcomponent.h"
#include "misc/statusmessage.h"
#include "misc/logmessage.h"
#include <QFileDialog>

using namespace swift::misc;

namespace BlackGui::Components
{
    CModelConverterXSetupComponent::CModelConverterXSetupComponent(QWidget *parent) : QFrame(parent),
                                                                                      ui(new Ui::CModelConverterXSetupComponent)
    {
        ui->setupUi(this);
        connect(ui->pb_DirBrowser, &QPushButton::clicked, this, &CModelConverterXSetupComponent::selectBinary);
        connect(ui->le_McxBinary, &QLineEdit::returnPressed, this, &CModelConverterXSetupComponent::saveSettings);
        ui->le_McxBinary->setText(m_setting.get());
    }

    CModelConverterXSetupComponent::~CModelConverterXSetupComponent()
    {}

    void CModelConverterXSetupComponent::selectBinary()
    {
        QString defaultValue = m_setting.get();
        if (defaultValue.isEmpty())
        {
            defaultValue = QDir::currentPath();
        }
        const QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                              tr("ModelConverterX binary"), defaultValue,
                                                              "*.exe");
        ui->le_McxBinary->setText(fileName);
        this->saveSettings();
    }

    void CModelConverterXSetupComponent::saveSettings()
    {
        const QString t = ui->le_McxBinary->text();
        const CStatusMessage msg = m_setting.setAndSave(t);
        CLogMessage::preformatted(msg);
    }
} // ns
