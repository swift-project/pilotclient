/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_ABOUTDIALOG_H
#define BLACKGUI_COMPONENTS_ABOUTDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CAboutDialog;
}
namespace BlackGui::Components
{
    /*!
     * About dialog
     */
    class CAboutDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAboutDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAboutDialog() override;

    private:
        //! Init values
        void init();

        QScopedPointer<Ui::CAboutDialog> ui;
    };
} // ns

#endif // guard
