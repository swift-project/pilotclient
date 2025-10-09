// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_AUTOPUBLISHDIALOG_H
#define SWIFT_GUI_COMPONENTS_AUTOPUBLISHDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"

namespace Ui
{
    class CAutoPublishDialog;
}
namespace swift::gui::components
{
    //! CAutoPublishComponent as dialog
    class SWIFT_GUI_EXPORT CAutoPublishDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CAutoPublishDialog(QWidget *parent = nullptr);

        //! Destructor
        ~CAutoPublishDialog() override;

        //! \copydoc CAutoPublishComponent::readFiles
        int readFiles();

        //! Read files and show dialog
        int readAndShow();

    private:
        QScopedPointer<Ui::CAutoPublishDialog> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_AUTOPUBLISHDIALOG_H
