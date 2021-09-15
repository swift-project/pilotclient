/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SIMBRIEFDOWNLOADDIALOG_H
#define BLACKGUI_COMPONENTS_SIMBRIEFDOWNLOADDIALOG_H

#include "blackmisc/aviation/simbriefdata.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CSimBriefDownloadDialog; }
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
        BlackMisc::Aviation::CSimBriefData getSimBriefData() const;

        //! Set UI values from data
        void setSimBriefData(const BlackMisc::Aviation::CSimBriefData &data);

        //! \copydoc QDialog::exec
        virtual int exec() override;

    private:
        QScopedPointer<Ui::CSimBriefDownloadDialog> ui;
        BlackMisc::CData<BlackMisc::Aviation::Data::TSimBriefData> m_simBrief { this };
    };
} // ns

#endif // guard
