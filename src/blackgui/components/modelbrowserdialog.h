/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
