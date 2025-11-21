// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETUPLOADINGDIALOG_H
#define SWIFT_GUI_COMPONENTS_SETUPLOADINGDIALOG_H

#include <QDialog>

#include "misc/statusmessagelist.h"

namespace Ui
{
    class CSetupLoadingDialog;
}
namespace swift::gui::components
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
        ~CSetupLoadingDialog() override;

    private:
        QScopedPointer<Ui::CSetupLoadingDialog> ui;

        //! Ctor
        explicit CSetupLoadingDialog(QWidget *parent = nullptr);
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_SETUPLOADINGDIALOG_H
