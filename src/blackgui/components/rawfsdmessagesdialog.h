// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_RAWFSDMESSAGESDIALOG_H
#define BLACKGUI_COMPONENTS_RAWFSDMESSAGESDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/rawfsdmessagescomponent.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CRawFsdMessagesDialog;
}
namespace BlackGui::Components
{
    //! CRawFsdMessageComponent as dialog
    class BLACKGUI_EXPORT CRawFsdMessagesDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CRawFsdMessagesDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CRawFsdMessagesDialog();

    private:
        QScopedPointer<Ui::CRawFsdMessagesDialog> ui;
    };
} // ns

#endif // guard
