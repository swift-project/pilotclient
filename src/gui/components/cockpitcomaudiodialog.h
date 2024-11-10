// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_COCKPITCOMAUDIODIALOG_H
#define SWIFT_GUI_COMPONENTS_COCKPITCOMAUDIODIALOG_H

#include "gui/swiftguiexport.h"
#include <QDialog>

namespace Ui
{
    class CCockpitComAudioDialog;
}
namespace swift::gui::components
{
    //! Cockpit COM component as dialog
    class SWIFT_GUI_EXPORT CCockpitComAudioDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CCockpitComAudioDialog(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CCockpitComAudioDialog() override;

    private:
        QScopedPointer<Ui::CCockpitComAudioDialog> ui;
    };
} // ns

#endif // guard
