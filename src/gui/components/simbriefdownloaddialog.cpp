// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simbriefdownloaddialog.h"
#include "ui_simbriefdownloaddialog.h"

using namespace swift::misc::aviation;

namespace swift::gui::components
{
    CSimBriefDownloadDialog::CSimBriefDownloadDialog(QWidget *parent) : QDialog(parent),
                                                                        ui(new Ui::CSimBriefDownloadDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

    CSimBriefDownloadDialog::~CSimBriefDownloadDialog()
    {}

    CSimBriefData CSimBriefDownloadDialog::getSimBriefData() const
    {
        return CSimBriefData(ui->le_SimBriefURL->text().trimmed(), ui->le_SimBriefUsername->text().trimmed());
    }

    void CSimBriefDownloadDialog::setSimBriefData(const CSimBriefData &data)
    {
        ui->le_SimBriefURL->setText(data.getUrl());
        ui->le_SimBriefUsername->setText(data.getUsername());
    }

    int CSimBriefDownloadDialog::exec()
    {
        this->setSimBriefData(m_simBrief.get());
        const int r = QDialog::exec();
        if (r == Accepted)
        {
            m_simBrief.set(this->getSimBriefData());
        }
        return r;
    }
} // ns
