// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

        void loadSwiftLicense();
        void loadThirdPartyLicenses();

        QScopedPointer<Ui::CAboutDialog> ui;
    };
} // ns

#endif // guard
