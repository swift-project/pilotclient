// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_ABOUTDIALOG_H
#define SWIFT_GUI_COMPONENTS_ABOUTDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CAboutDialog;
}
namespace swift::gui::components
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
        ~CAboutDialog() override;

    private:
        //! Init values
        void init();

        void loadSwiftLicense();
        void loadThirdPartyLicenses();

        QScopedPointer<Ui::CAboutDialog> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_ABOUTDIALOG_H
