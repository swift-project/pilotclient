// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_INSTALLXSWIFTBUSDIALOG_H
#define SWIFT_GUI_COMPONENTS_INSTALLXSWIFTBUSDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui
{
    class CInstallXSwiftBusDialog;
}
namespace swift::gui::components
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
} // namespace swift::gui::components

#endif // guard
