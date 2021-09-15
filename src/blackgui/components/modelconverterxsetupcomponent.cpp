/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "modelconverterxsetupcomponent.h"
#include "ui_modelconverterxsetupcomponent.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/logmessage.h"
#include <QFileDialog>

using namespace BlackMisc;

namespace BlackGui::Components
{
    CModelConverterXSetupComponent::CModelConverterXSetupComponent(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CModelConverterXSetupComponent)
    {
        ui->setupUi(this);
        connect(ui->pb_DirBrowser, &QPushButton::clicked, this, &CModelConverterXSetupComponent::selectBinary);
        connect(ui->le_McxBinary, &QLineEdit::returnPressed, this, &CModelConverterXSetupComponent::saveSettings);
        ui->le_McxBinary->setText(m_setting.get());
    }

    CModelConverterXSetupComponent::~CModelConverterXSetupComponent()
    { }

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
