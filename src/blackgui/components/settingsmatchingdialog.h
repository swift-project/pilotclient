/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSMATCHINGDIALOG_H
#define BLACKGUI_COMPONENTS_SETTINGSMATCHINGDIALOG_H

#include "blackmisc/simulation/aircraftmatchersetup.h"
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
        BlackMisc::Simulation::CAircraftMatcherSetup getMatchingSetup() const;

        //! Set the setup
        void setMatchingSetup(const BlackMisc::Simulation::CAircraftMatcherSetup &setup);

    private:
        QScopedPointer<Ui::CSettingsMatchingDialog> ui;
    };
} // ns

#endif // guard
