// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_MODELBROWSERDIALOG_H
#define BLACKGUI_COMPONENTS_MODELBROWSERDIALOG_H

#include "blackgui/blackguiexport.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CModelBrowserDialog;
}
namespace BlackGui::Components
{
    //! Model browser as dialog
    class BLACKGUI_EXPORT CModelBrowserDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CModelBrowserDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CModelBrowserDialog() override;

    protected:
        //! \copydoc QObject::event
        virtual bool event(QEvent *event) override;

        //! \copydoc QDialog::closeEvent
        virtual void done(int r) override;

    private:
        QScopedPointer<Ui::CModelBrowserDialog> ui;
    };
} // ns

#endif // guard
