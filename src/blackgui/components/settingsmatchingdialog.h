// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSMATCHINGDIALOG_H
#define BLACKGUI_COMPONENTS_SETTINGSMATCHINGDIALOG_H

#include "misc/simulation/aircraftmatchersetup.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CSettingsMatchingDialog;
}
namespace BlackGui::Components
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
} // ns

#endif // guard
