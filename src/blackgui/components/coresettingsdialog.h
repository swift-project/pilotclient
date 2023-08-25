// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_CORESETTINGSDIALOG_H
#define BLACKGUI_COMPONENTS_CORESETTINGSDIALOG_H

#include "blackgui/blackguiexport.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CCoreSettingsDialog;
}
namespace BlackGui::Components
{
    /*!
     * Core / swift pilot client settings as Dialog
     */
    class BLACKGUI_EXPORT CCoreSettingsDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCoreSettingsDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCoreSettingsDialog();

    private:
        QScopedPointer<Ui::CCoreSettingsDialog> ui;

        //! Show the overview tab
        void showOverview();
    };
} // ns

#endif // guard
