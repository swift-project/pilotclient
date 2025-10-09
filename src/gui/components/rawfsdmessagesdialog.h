// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_RAWFSDMESSAGESDIALOG_H
#define SWIFT_GUI_COMPONENTS_RAWFSDMESSAGESDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "gui/components/rawfsdmessagescomponent.h"
#include "gui/swiftguiexport.h"

namespace Ui
{
    class CRawFsdMessagesDialog;
}
namespace swift::gui::components
{
    //! CRawFsdMessageComponent as dialog
    class SWIFT_GUI_EXPORT CRawFsdMessagesDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CRawFsdMessagesDialog(QWidget *parent = nullptr);

        //! Destructor
        ~CRawFsdMessagesDialog() override;

    private:
        QScopedPointer<Ui::CRawFsdMessagesDialog> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_RAWFSDMESSAGESDIALOG_H
