// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_FIRSTMODELSETDIALOG_H
#define SWIFT_GUI_COMPONENTS_FIRSTMODELSETDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CFirstModelSetDialog;
}
namespace swift::gui::components
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
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_FIRSTMODELSETDIALOG_H
