// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_AUDIOADVANCEDDISTRIBUTEDDIALOG_H
#define SWIFT_GUI_COMPONENTS_AUDIOADVANCEDDISTRIBUTEDDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"

namespace Ui
{
    class CAudioAdvancedDistributedDialog;
}
namespace swift::gui::components
{
    //! Audio advanced setup as dialog
    class SWIFT_GUI_EXPORT CAudioAdvancedDistributedDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CAudioAdvancedDistributedDialog(QWidget *parent = nullptr);

        //! Dtor
        ~CAudioAdvancedDistributedDialog() override;

        //! Reload registered devices
        void reloadRegisteredDevices();

    private:
        QScopedPointer<Ui::CAudioAdvancedDistributedDialog> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_AUDIOADVANCEDDISTRIBUTEDDIALOG_H
