/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simbriefdownloaddialog.h"
#include "ui_simbriefdownloaddialog.h"

using namespace BlackMisc::Aviation;

namespace BlackGui::Components
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
