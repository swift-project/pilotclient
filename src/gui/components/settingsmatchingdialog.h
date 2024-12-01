// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSMATCHINGDIALOG_H
#define SWIFT_GUI_COMPONENTS_SETTINGSMATCHINGDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "misc/simulation/aircraftmatchersetup.h"

namespace Ui
{
    class CSettingsMatchingDialog;
}
namespace swift::gui::components
{
    //! Matching settings as dialog
    class CSettingsMatchingDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Matching settings as dialog
        explicit CSettingsMatchingDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsMatchingDialog();

        //! Get setup
        swift::misc::simulation::CAircraftMatcherSetup getMatchingSetup() const;

        //! Set the setup
        void setMatchingSetup(const swift::misc::simulation::CAircraftMatcherSetup &setup);

    private:
        QScopedPointer<Ui::CSettingsMatchingDialog> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_SETTINGSMATCHINGDIALOG_H
