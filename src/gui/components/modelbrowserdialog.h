// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_MODELBROWSERDIALOG_H
#define SWIFT_GUI_COMPONENTS_MODELBROWSERDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"

namespace Ui
{
    class CModelBrowserDialog;
}
namespace swift::gui::components
{
    //! Model browser as dialog
    class SWIFT_GUI_EXPORT CModelBrowserDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CModelBrowserDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CModelBrowserDialog() override;

        //! \copydoc QDialog::closeEvent
        virtual void done(int r) override;

    private:
        QScopedPointer<Ui::CModelBrowserDialog> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_MODELBROWSERDIALOG_H
