// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETUPLOADINGDIALOG_H
#define BLACKGUI_COMPONENTS_SETUPLOADINGDIALOG_H

#include "misc/statusmessagelist.h"
#include <QDialog>

namespace Ui
{
    class CSetupLoadingDialog;
}
namespace BlackGui::Components
{
    /*!
     * Setup dialog, if loading the boostrap file fails
     */
    class CSetupLoadingDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Ctor with messages
        CSetupLoadingDialog(const swift::misc::CStatusMessageList &msgs, QWidget *parent = nullptr);

        //! Dtor
        virtual ~CSetupLoadingDialog();

    private:
        QScopedPointer<Ui::CSetupLoadingDialog> ui;

        //! Ctor
        explicit CSetupLoadingDialog(QWidget *parent = nullptr);
    };
} // ns

#endif // guard
