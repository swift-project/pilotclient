// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_INSTALLXSWIFTBUSDIALOG_H
#define BLACKGUI_COMPONENTS_INSTALLXSWIFTBUSDIALOG_H

#include <QString>
#include <QDialog>

namespace Ui
{
    class CInstallXSwiftBusDialog;
}
namespace BlackGui::Components
{
    /*!
     * CInstallXSwiftBusComponent as dialog
     */
    class CInstallXSwiftBusDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInstallXSwiftBusDialog(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CInstallXSwiftBusDialog() override;

        //! \copydoc CInstallXSwiftBusComponent::setDefaultDownloadName
        void setDefaultDownloadName(const QString &defaultName);

    private:
        QScopedPointer<Ui::CInstallXSwiftBusDialog> ui;
    };
} // ns

#endif // guard
