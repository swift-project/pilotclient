// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_FIRSTMODELSETDIALOG_H
#define BLACKGUI_COMPONENTS_FIRSTMODELSETDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CFirstModelSetDialog;
}
namespace BlackGui::Components
{
    /*!
     * First model set dialog
     */
    class CFirstModelSetDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CFirstModelSetDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CFirstModelSetDialog();

    private:
        QScopedPointer<Ui::CFirstModelSetDialog> ui;
    };
} // ns
#endif // guard
