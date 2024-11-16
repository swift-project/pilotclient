// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_CORESETTINGSDIALOG_H
#define SWIFT_GUI_COMPONENTS_CORESETTINGSDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"

namespace Ui
{
    class CCoreSettingsDialog;
}
namespace swift::gui::components
{
    /*!
     * Core / swift pilot client settings as Dialog
     */
    class SWIFT_GUI_EXPORT CCoreSettingsDialog : public QDialog
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
} // namespace swift::gui::components

#endif // guard
