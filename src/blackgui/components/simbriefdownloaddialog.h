// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SIMBRIEFDOWNLOADDIALOG_H
#define BLACKGUI_COMPONENTS_SIMBRIEFDOWNLOADDIALOG_H

#include "misc/aviation/simbriefdata.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CSimBriefDownloadDialog;
}
namespace BlackGui::Components
{
    //! Download from SimBrief
    class CSimBriefDownloadDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CSimBriefDownloadDialog(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CSimBriefDownloadDialog() override;

        //! SimBrief data
        swift::misc::aviation::CSimBriefData getSimBriefData() const;

        //! Set UI values from data
        void setSimBriefData(const swift::misc::aviation::CSimBriefData &data);

        //! \copydoc QDialog::exec
        virtual int exec() override;

    private:
        QScopedPointer<Ui::CSimBriefDownloadDialog> ui;
        swift::misc::CData<swift::misc::aviation::Data::TSimBriefData> m_simBrief { this };
    };
} // ns

#endif // guard
